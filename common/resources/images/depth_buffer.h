#ifndef DEPTH_BUFFER_H
#define DEPTH_BUFFER_H

#include <vulkan/vulkan_core.h>

#include "resources/images/image.h"

namespace vkcommon
{
    class Device;
    class MemoryAllocator;
    class SwapChain;

    class DepthBuffer
    {
    public:
        DepthBuffer(const Device& device, MemoryAllocator& allocator);

        ~DepthBuffer();

        // Disable copying
        DepthBuffer(const DepthBuffer&) = delete;
        DepthBuffer& operator=(const DepthBuffer&) = delete;

        // Enable moving
        DepthBuffer(DepthBuffer&& other) noexcept;
        DepthBuffer& operator=(DepthBuffer&& other) noexcept;

        void create(const SwapChain& swapChain);

        VkImageView imageView() const { return m_imageView; }
        VkFormat format() const { return m_format; }
        VkImage handle() const { return m_image.handle(); }

    private:
        void cleanup();
  
        Image m_image;

        VkDeviceMemory m_imageMemory;
        VkImageView m_imageView;
        VkFormat m_format;

        const Device& m_deviceRef;
        MemoryAllocator& m_allocatorRef;
    };
} // namespace vkcommon

#endif // DEPTH_BUFFER_H

