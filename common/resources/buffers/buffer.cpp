#include "buffer.h"

#include "core/device.h"
#include "graphics/command_pool.h"
#include "resources/memory/memory_allocator.h"

#include <stdexcept>

namespace vkcommon {

    Buffer::Buffer(const Device& device, MemoryAllocator& allocator)
        : m_deviceRef(device)
        , m_allocatorRef(allocator) {
    }

    Buffer::~Buffer() {
        cleanup();
    }

    void Buffer::cleanup() {
        if (m_buffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(m_deviceRef.handle(), m_buffer, nullptr);
            m_buffer = VK_NULL_HANDLE;
        }

        if (m_memory != VK_NULL_HANDLE) {
            m_allocatorRef.freeMemory(m_memory);
            m_memory = VK_NULL_HANDLE;
        }

        m_size = 0;
    }

    Buffer::Buffer(Buffer&& other) noexcept
        : m_deviceRef(other.m_deviceRef)
        , m_allocatorRef(other.m_allocatorRef)
        , m_buffer(other.m_buffer)
        , m_memory(other.m_memory)
        , m_size(other.m_size) {
        other.m_buffer = VK_NULL_HANDLE;
        other.m_memory = VK_NULL_HANDLE;
        other.m_size = 0;
    }

    Buffer& Buffer::operator=(Buffer&& other) noexcept {
        if (this != &other) {
            cleanup();

            m_buffer = other.m_buffer;
            m_memory = other.m_memory;
            m_size = other.m_size;

            other.m_buffer = VK_NULL_HANDLE;
            other.m_memory = VK_NULL_HANDLE;
            other.m_size = 0;
        }
        return *this;
    }

    void Buffer::create(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    {
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = usage;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(m_deviceRef.handle(), &bufferCreateInfo, nullptr, &m_buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create vertex buffer!");
        }

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(m_deviceRef.handle(), m_buffer, &memoryRequirements);

        m_allocatorRef.allocateMemoryForRequirements(memoryRequirements, properties);
        m_size = size;

        vkBindBufferMemory(m_deviceRef.handle(), m_buffer, m_memory, 0);
    }

    void Buffer::copyFrom(const Buffer& srcBuffer, VkDeviceSize size, const CommandPool& cmdPool)
    {

        VkCommandBuffer commandBuffer = cmdPool.beginSingleTimeCommand();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer.handle(), m_buffer, 1, &copyRegion);

        cmdPool.endSingleTimeCommand(commandBuffer, m_deviceRef.graphicsQueue());
    }

    void Buffer::copyTo(void* data, VkDeviceSize size) const {
        void* mapped;
        if (vkMapMemory(m_deviceRef.handle(), m_memory, 0, size, 0, &mapped) != VK_SUCCESS) {
            throw std::runtime_error("Failed to map memory");
        }
        memcpy(data, mapped, size);
        vkUnmapMemory(m_deviceRef.handle(), m_memory);
    }

    void Buffer::update(const void* data, VkDeviceSize size, VkDeviceSize offset) {
        void* mapped;
        if (vkMapMemory(m_deviceRef.handle(), m_memory, offset, size, 0, &mapped) != VK_SUCCESS) {
            throw std::runtime_error("Failed to map memory");
        }
        memcpy(mapped, data, size);
        vkUnmapMemory(m_deviceRef.handle(), m_memory);
    }

} //namespace vkcommon