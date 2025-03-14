#include "uniform_buffer.h"

#include "core/device.h"
#include <stdexcept>

namespace vkcommon {

    UniformBuffer::UniformBuffer(const Device& device, MemoryAllocator& allocator)
        : m_device(device)
        , m_allocator(allocator) {
    }

    void UniformBuffer::create(VkDeviceSize bufferSize, uint32_t framesInFlight) {
        m_bufferSize = bufferSize;
        m_buffers.clear();
        m_buffers.reserve(framesInFlight);
        for (uint32_t i = 0; i < framesInFlight; ++i) {
            m_buffers.emplace_back(m_device, m_allocator);
            m_buffers.back().create(
                bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );
        }
    }

    void UniformBuffer::updateData(uint32_t currentFrame, const void* data, VkDeviceSize size) {
        if (currentFrame >= m_buffers.size()) {
            throw std::runtime_error("Frame index out of range");
        }

        if (size > m_bufferSize) {
            throw std::runtime_error("Update size larger than buffer size");
        }

        m_buffers[currentFrame].update(data, size);
    }

} // namespace vkcommon