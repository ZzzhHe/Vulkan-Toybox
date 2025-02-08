#ifndef IMAGE_H
#define IMAGE_H

#include <vulkan/vulkan_core.h>

namespace vkcommon {
    class Device;
    class CommandPool;
    class Buffer;
    class MemoryAllocator;

    class Image {
    public:
        Image(const Device& device, MemoryAllocator& allocator);
        virtual ~Image();

        // Disable copying
        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;

        // Enable moving
        Image(Image&& other) noexcept;
        Image& operator=(Image&& other) noexcept;

        void create(uint32_t width,
            uint32_t height,
            uint32_t mipLevels,
            VkSampleCountFlagBits numSamples,
            VkFormat format,
            VkImageTiling tiling,
            VkImageUsageFlags usage,
            VkMemoryPropertyFlags properties);

        void transitionLayout(VkImageLayout newLayout,
            const CommandPool& cmdPool);

        void copyFromBuffer(const Buffer& buffer,
            uint32_t width,
            uint32_t height,
            const CommandPool& cmdPool);

        VkImageView createView(VkFormat format,
            VkImageAspectFlags aspectFlags,
            uint32_t mipLevels = 1);

        VkImage handle() const { return m_image; }
        VkDeviceMemory memory() const { return m_memory; }
        VkFormat format() const { return m_format; }
        uint32_t mipLevels() const { return m_mipLevels; }
        VkExtent2D extent() const { return m_extent; }

    protected:
        const Device& m_deviceRef;
        MemoryAllocator& m_allocatorRef;

    private:
        VkImage m_image{ VK_NULL_HANDLE };
        VkDeviceMemory m_memory{ VK_NULL_HANDLE };
        VkFormat m_format{ VK_FORMAT_UNDEFINED };
        uint32_t m_mipLevels{ 1 };
        VkExtent2D m_extent{ 0, 0 };
        VkImageLayout m_currentLayout{ VK_IMAGE_LAYOUT_UNDEFINED };

        void cleanup();
    };

} // namespace vkcommon

#endif //IMAGE_H
