#include "color_image.h"

#include "core/device.h"
#include "core/physical_device.h"
#include "graphics/swap_chain.h"
#include "resources/memory/memory_allocator.h"

#include <stdexcept>

namespace vkcommon {

    ColorImage::ColorImage(const Device& device, MemoryAllocator& allocator)
        : m_image(device, allocator)
        , m_deviceRef(device) {
    }

    ColorImage::~ColorImage() {
        cleanup();
    }

    ColorImage::ColorImage(ColorImage&& other) noexcept
        : m_image(std::move(other.m_image))
        , m_imageView(other.m_imageView)
        , m_format(other.m_format)
        , m_samples(other.m_samples)
        , m_deviceRef(other.m_deviceRef) {
        other.m_imageView = VK_NULL_HANDLE;
        other.m_format = VK_FORMAT_UNDEFINED;
        other.m_samples = VK_SAMPLE_COUNT_1_BIT;
    }

    ColorImage& ColorImage::operator=(ColorImage&& other) noexcept {
        if (this != &other) {
            cleanup();

            m_image = std::move(other.m_image);
            m_imageView = other.m_imageView;
            m_format = other.m_format;
            m_samples = other.m_samples;

            other.m_imageView = VK_NULL_HANDLE;
            other.m_format = VK_FORMAT_UNDEFINED;
            other.m_samples = VK_SAMPLE_COUNT_1_BIT;
        }
        return *this;
    }

    void ColorImage::cleanup() {
        if (m_imageView != VK_NULL_HANDLE) {
            vkDestroyImageView(m_deviceRef.handle(), m_imageView, nullptr);
            m_imageView = VK_NULL_HANDLE;
        }
    }

    void ColorImage::create(const SwapChain& swapChain) {
        // Store format and sample count
        m_format = swapChain.swapChainImageFormat();
        m_samples = m_deviceRef.msaaSamples();

        const auto& extent = swapChain.swapChainExtent();

        // Create color image with MSAA
        m_image.create(
            extent.width,
            extent.height,
            1,                          // mipLevels
            m_samples,                  // MSAA sample count
            m_format,                   // Format matching swapchain
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        createImageView();
    }

    void ColorImage::createImageView() {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_image.handle();
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = m_format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_deviceRef.handle(), &viewInfo, nullptr, &m_imageView) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create color image view!");
        }
    }

} // namespace vkcommon