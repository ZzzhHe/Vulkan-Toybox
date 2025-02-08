#include "memory_allocator.h"

#include "core/physical_device.h"
#include "core/device.h"

#include <stdexcept>

namespace vkcommon {
    MemoryAllocator::MemoryAllocator(const PhysicalDevice& physicalDevice, const Device& device)
        : m_deviceRef(device) {
        vkGetPhysicalDeviceMemoryProperties(physicalDevice.handle(), &m_memProperties);
    }

    uint32_t MemoryAllocator::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
        for (uint32_t i = 0; i < m_memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) &&
                (m_memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        throw std::runtime_error("Failed to find suitable memory type");
    }

    VkDeviceMemory MemoryAllocator::allocateMemory(VkDeviceSize size, uint32_t memoryTypeIndex) const {
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = size;
        allocInfo.memoryTypeIndex = memoryTypeIndex;

        VkDeviceMemory memory;
        if (vkAllocateMemory(m_deviceRef.handle(), &allocInfo, nullptr, &memory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate memory");
        }
        return memory;
    }

    void MemoryAllocator::freeMemory(VkDeviceMemory memory) const {
        if (memory != VK_NULL_HANDLE) {
            vkFreeMemory(m_deviceRef.handle(), memory, nullptr);
        }
    }

    VkDeviceMemory MemoryAllocator::allocateMemoryForRequirements(
        const VkMemoryRequirements& memRequirements,
        VkMemoryPropertyFlags properties) const {

        uint32_t memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
        return allocateMemory(memRequirements.size, memoryTypeIndex);
    }

} // namespace vkcommon