#include "depth_buffer.h"

#include "core/device.h"
#include "graphics/swap_chain.h"
#include "resources/images/image.h"

namespace vkcommon
{
    DepthBuffer::DepthBuffer(const Device& device, MemoryAllocator& allocator)
        : m_deviceRef(device), m_allocatorRef(allocator), m_image(device, allocator)
    {
    }

    DepthBuffer::~DepthBuffer()
    {
        cleanup();
    }

    void DepthBuffer::cleanup() {
        if (m_imageView != VK_NULL_HANDLE) {
            vkDestroyImageView(m_deviceRef.handle(), m_imageView, nullptr);
            m_imageView = VK_NULL_HANDLE;
        }
    }

    DepthBuffer::DepthBuffer(DepthBuffer&& other) noexcept
        : m_deviceRef(other.m_deviceRef)
        , m_allocatorRef(other.m_allocatorRef)
        , m_image(std::move(other.m_image))
        , m_imageView(other.m_imageView)
        , m_format(other.m_format) {
        other.m_imageView = VK_NULL_HANDLE;
        other.m_format = VK_FORMAT_UNDEFINED;
    }

    DepthBuffer& DepthBuffer::operator=(DepthBuffer&& other) noexcept {
        if (this != &other) {
            cleanup();

            m_image = std::move(other.m_image);
            m_imageView = other.m_imageView;
            m_format = other.m_format;

            other.m_imageView = VK_NULL_HANDLE;
            other.m_format = VK_FORMAT_UNDEFINED;
        }
        return *this;
    }

    void DepthBuffer::create(const SwapChain& swapChain) {
        m_format = m_deviceRef.findDepthFormat();
        const auto& extent = swapChain.swapChainExtent();

        m_image.create(
            extent.width,
            extent.height,
            1,
            m_deviceRef.msaaSamples(),
            m_format,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        m_imageView = m_image.createView(m_format, VK_IMAGE_ASPECT_DEPTH_BIT);
    }
}
