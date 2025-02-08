#include "image.h"

#include "core/device.h"
#include "graphics/command_pool.h"
#include "resources/buffers/buffer.h"
#include "resources/memory/memory_allocator.h"

#include <stdexcept>

namespace vkcommon {

    Image::Image(const Device& device, MemoryAllocator& allocator)
        : m_deviceRef(device), m_allocatorRef(allocator) {
    }

    Image::~Image() {
        cleanup();
    }

    void Image::cleanup() {
        if (m_image != VK_NULL_HANDLE) {
            vkDestroyImage(m_deviceRef.handle(), m_image, nullptr);
            m_image = VK_NULL_HANDLE;
        }

        if (m_memory != VK_NULL_HANDLE) {
            m_allocatorRef.freeMemory(m_memory);
            m_memory = VK_NULL_HANDLE;
        }

        m_format = VK_FORMAT_UNDEFINED;
        m_mipLevels = 1;
        m_extent = { 0, 0 };
        m_currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }

    Image::Image(Image&& other) noexcept
        : m_deviceRef(other.m_deviceRef)
        , m_allocatorRef(other.m_allocatorRef)
        , m_image(other.m_image)
        , m_memory(other.m_memory)
        , m_format(other.m_format)
        , m_mipLevels(other.m_mipLevels)
        , m_extent(other.m_extent)
        , m_currentLayout(other.m_currentLayout) {
        other.m_image = VK_NULL_HANDLE;
        other.m_memory = VK_NULL_HANDLE;
        other.m_format = VK_FORMAT_UNDEFINED;
        other.m_mipLevels = 1;
        other.m_extent = { 0, 0 };
        other.m_currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }

    Image& Image::operator=(Image&& other) noexcept {
        if (this != &other) {
            cleanup();

            m_image = other.m_image;
            m_memory = other.m_memory;
            m_format = other.m_format;
            m_mipLevels = other.m_mipLevels;
            m_extent = other.m_extent;
            m_currentLayout = other.m_currentLayout;

            other.m_image = VK_NULL_HANDLE;
            other.m_memory = VK_NULL_HANDLE;
            other.m_format = VK_FORMAT_UNDEFINED;
            other.m_mipLevels = 1;
            other.m_extent = { 0, 0 };
            other.m_currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        }
        return *this;
    }

    void Image::create(uint32_t width, uint32_t height, uint32_t mipLevels,
        VkSampleCountFlagBits numSamples, VkFormat format,
        VkImageTiling tiling, VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties) {

        m_format = format;
        m_mipLevels = mipLevels;
        m_extent = { width, height };

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = numSamples;

        if (vkCreateImage(m_deviceRef.handle(), &imageInfo, nullptr, &m_image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_deviceRef.handle(), m_image, &memRequirements);

        m_memory = m_allocatorRef.allocateMemoryForRequirements(memRequirements, properties);

        if (vkBindImageMemory(m_deviceRef.handle(), m_image, m_memory, 0) != VK_SUCCESS) {
            throw std::runtime_error("Failed to bind image memory");
        }
    }

    void Image::transitionLayout(VkImageLayout newLayout, const CommandPool& cmdPool) {
        VkCommandBuffer commandBuffer = cmdPool.beginSingleTimeCommand();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = m_currentLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = m_image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = m_mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (m_currentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (m_currentLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        cmdPool.endSingleTimeCommand(commandBuffer, m_deviceRef.graphicsQueue());
        m_currentLayout = newLayout;
    }

    void Image::copyFromBuffer(const Buffer& buffer, uint32_t width, uint32_t height,
        const CommandPool& cmdPool) {
        VkCommandBuffer commandBuffer = cmdPool.beginSingleTimeCommand();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(commandBuffer, buffer.handle(), m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        cmdPool.endSingleTimeCommand(commandBuffer, m_deviceRef.graphicsQueue());
    }

    VkImageView Image::createView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        if (vkCreateImageView(m_deviceRef.handle(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image view!");
        }

        return imageView;
    }

} // namespace vkcommon