#include "texture.h"

#include "core/physical_device.h"
#include "core/device.h"
#include "graphics/command_pool.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdexcept>
#include <cmath>

namespace vkcommon {
    Texture::Texture(const Device& device, MemoryAllocator& allocator)
        : m_deviceRef(device), m_allocatorRef(allocator), m_image(device, allocator) {
    }

    Texture::~Texture() {
        cleanup();
    }

    void Texture::cleanup() {
        if (m_sampler != VK_NULL_HANDLE) {
            vkDestroySampler(m_deviceRef.handle(), m_sampler, nullptr);
            m_sampler = VK_NULL_HANDLE;
        }

        if (m_imageView != VK_NULL_HANDLE) {
            vkDestroyImageView(m_deviceRef.handle(), m_imageView, nullptr);
            m_imageView = VK_NULL_HANDLE;
        }

        m_stagingBuffer.reset();
    }

    Texture::Texture(Texture&& other) noexcept
        : m_deviceRef(other.m_deviceRef)
        , m_allocatorRef(other.m_allocatorRef)
        , m_image(std::move(other.m_image))
        , m_imageView(other.m_imageView)
        , m_sampler(other.m_sampler)
        , m_stagingBuffer(std::move(other.m_stagingBuffer)) {
        other.m_imageView = VK_NULL_HANDLE;
        other.m_sampler = VK_NULL_HANDLE;
    }

    Texture& Texture::operator=(Texture&& other) noexcept {
        if (this != &other) {
            cleanup();

            m_image = std::move(other.m_image);
            m_imageView = other.m_imageView;
            m_sampler = other.m_sampler;
            m_stagingBuffer = std::move(other.m_stagingBuffer);

            other.m_imageView = VK_NULL_HANDLE;
            other.m_sampler = VK_NULL_HANDLE;
        }
        return *this;
    }

    void Texture::loadFromFile(const std::string& filepath, const CommandPool& commandPool) {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(filepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }

        VkDeviceSize imageSize = texWidth * texHeight * 4;

        uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

        m_stagingBuffer = std::make_unique<Buffer>(m_deviceRef, m_allocatorRef);
        m_stagingBuffer->create(
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        m_stagingBuffer->update(pixels, imageSize);
        stbi_image_free(pixels);

        m_image.create(
            texWidth,
            texHeight,
            mipLevels,
            VK_SAMPLE_COUNT_1_BIT,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        // Transition image to be ready for copy
        m_image.transitionLayout(
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            commandPool
        );

        // Copy from staging buffer to image
        m_image.copyFromBuffer(*m_stagingBuffer, texWidth, texHeight, commandPool);

        // Generate mipmaps
        generateMipMaps(commandPool);

        // Create image view
        m_imageView = m_image.createView(
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_ASPECT_COLOR_BIT,
            mipLevels
        );

        // Cleanup staging buffer
        m_stagingBuffer.reset();
    }

    void Texture::createSampler(float maxAnisotropy = 16.0f,
        VkFilter minFilter = VK_FILTER_LINEAR,
        VkFilter magFilter = VK_FILTER_LINEAR,
        VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT) {

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = magFilter;
        samplerInfo.minFilter = minFilter;
        samplerInfo.addressModeU = addressMode;
        samplerInfo.addressModeV = addressMode;
        samplerInfo.addressModeW = addressMode;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = maxAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(m_image.mipLevels());
        samplerInfo.mipLodBias = 0.0f;

        if (vkCreateSampler(m_deviceRef.handle(), &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void Texture::generateMipMaps(const CommandPool& cmdPool) {
        VkFormatProperties formatProperties = m_deviceRef.physicalDeviceFormatProperties(m_image.format());

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
            throw std::runtime_error("Texture image format does not support linear blitting!");
        }

        VkCommandBuffer commandBuffer = cmdPool.beginSingleTimeCommand();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = m_image.handle();
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = m_image.extent().width;
        int32_t mipHeight = m_image.extent().height;

        for (uint32_t i = 1; i < m_image.mipLevels(); i++) {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = {
                    mipWidth > 1 ? mipWidth / 2 : 1,
                    mipHeight > 1 ? mipHeight / 2 : 1,
                    1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(commandBuffer,
                m_image.handle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                m_image.handle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit,
                VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = m_image.mipLevels() - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        cmdPool.endSingleTimeCommand(commandBuffer, m_deviceRef.graphicsQueue());
    }
} // namespace vkcommon



