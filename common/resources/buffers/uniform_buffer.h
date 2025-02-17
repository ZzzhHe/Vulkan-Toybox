#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include "buffer.h"
#include <vector>

namespace vkcommon {

    class Device;
    class MemoryAllocator;

    class UniformBuffer {
    public:
        UniformBuffer(const Device& device, MemoryAllocator& allocator);
        virtual ~UniformBuffer() = default;

        // Disable copying
        UniformBuffer(const UniformBuffer&) = delete;
        UniformBuffer& operator=(const UniformBuffer&) = delete;

        // Enable moving
        UniformBuffer(UniformBuffer&& other) noexcept = default;
        UniformBuffer& operator=(UniformBuffer&& other) noexcept = default;

        // Create uniform buffers for specified number of frames
        void create(VkDeviceSize bufferSize, uint32_t framesInFlight);

        // Get buffer handle for descriptor binding
        VkBuffer buffer(uint32_t frame) const { return m_buffers[frame].handle(); }
        VkDeviceSize size() const { return m_bufferSize; }

        //protected:
            // Helper method for derived classes to update data
        void updateData(uint32_t currentFrame, const void* data, VkDeviceSize size);

    private:
        const Device& m_device;
        MemoryAllocator& m_allocator;

        std::vector<Buffer> m_buffers;  // One per frame in flight
        VkDeviceSize m_bufferSize{ 0 };
    };

} // namespace vkcommon

#endif // UNIFORM_BUFFER_H