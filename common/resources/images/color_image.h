#ifndef COLOR_IMAGE_H
#define COLOR_IMAGE_H

#include <vulkan/vulkan.h>
#include "resources/images/image.h"

namespace vkcommon {
    class Device;
    class MemoryAllocator;
    class SwapChain;

    class ColorImage {
    public:
        ColorImage(const Device& device, MemoryAllocator& allocator);
        ~ColorImage();

        // Disable copying
        ColorImage(const ColorImage&) = delete;
        ColorImage& operator=(const ColorImage&) = delete;

        // Enable moving
        ColorImage(ColorImage&& other) noexcept;
        ColorImage& operator=(ColorImage&& other) noexcept;

        // Create MSAA color buffer matching swapchain format
        void create(const SwapChain& swapChain);

        // Access methods
        VkImage handle() const { return m_image.handle(); }
        VkImageView imageView() const { return m_imageView; }
        VkFormat format() const { return m_format; }
        VkSampleCountFlagBits samples() const { return m_samples; }

    private:
        void cleanup();
        void createImageView();

    private:
        Image m_image;                        // Underlying image resource
        VkImageView m_imageView{ VK_NULL_HANDLE };  // Image view for the color buffer
        VkFormat m_format{ VK_FORMAT_UNDEFINED };   // Format matching swapchain
        VkSampleCountFlagBits m_samples{ VK_SAMPLE_COUNT_1_BIT };  // MSAA sample count

        const Device& m_deviceRef;           // Device reference for resource creation/cleanup
    };

} // namespace vkcommon

#endif // COLOR_IMAGE_H