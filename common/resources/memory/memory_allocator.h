#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

#include <vulkan/vulkan.h>

namespace vkcommon {

    class PhysicalDevice;
    class Device;

    class MemoryAllocator {
    public:
        explicit MemoryAllocator(const PhysicalDevice& physicalDevice, const Device& device);
        ~MemoryAllocator() = default;

        // Disable copying
        MemoryAllocator(const MemoryAllocator&) = delete;
        MemoryAllocator& operator=(const MemoryAllocator&) = delete;

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

        VkDeviceMemory allocateMemory(VkDeviceSize size, uint32_t memoryTypeIndex) const;
        void freeMemory(VkDeviceMemory memory) const;

        VkDeviceMemory allocateMemoryForRequirements(
            const VkMemoryRequirements& memRequirements,
            VkMemoryPropertyFlags properties) const;
    private:
        const Device& m_deviceRef;
        VkPhysicalDeviceMemoryProperties m_memProperties;
    };
} // namespace vkcommon

#endif // MEMORY_ALLOCATOR_H
