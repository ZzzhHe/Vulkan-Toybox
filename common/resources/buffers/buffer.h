#ifndef BUFFER_H
#define BUFFER_H
#include <vulkan/vulkan_core.h>

namespace vkcommon {

    class Device;
    class CommandPool;
    class MemoryAllocator;

    class Buffer
    {
    public:
        Buffer(const Device& device, MemoryAllocator& allocator);
        ~Buffer();

        // Disable copying
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        // Enable moving
        Buffer(Buffer&& other) noexcept;
        Buffer& operator=(Buffer&& other) noexcept;

        void create(VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties);

        void copyFrom(const Buffer& srcBuffer, VkDeviceSize size, const CommandPool& cmdPool);
        void copyTo(void* data, VkDeviceSize size) const;
        void update(const void* data, VkDeviceSize size, VkDeviceSize offset = 0);

        VkBuffer handle() const { return m_buffer; }
        VkDeviceMemory memory() const { return m_memory; }
        VkDeviceSize size() const { return m_size; }

    protected:
        const Device& m_deviceRef;
        MemoryAllocator& m_allocatorRef;

    private:
        void cleanup();

        VkBuffer m_buffer{ VK_NULL_HANDLE };
        VkDeviceMemory m_memory{ VK_NULL_HANDLE };
        VkDeviceSize m_size{ 0 };

    };

} // namespace vkcommon

#endif // BUFFER_H
