#ifndef DESCRIPTOR_POOL_H
#define DESCRIPTOR_POOL_H

#include <vulkan/vulkan.h>
#include <vector>

namespace vkcommon {

    class Device;

    class DescriptorPool {
    public:
        DescriptorPool(const Device& device);
        ~DescriptorPool();

        // Disable copying
        DescriptorPool(const DescriptorPool&) = delete;
        DescriptorPool& operator=(const DescriptorPool&) = delete;

        // Enable moving
        DescriptorPool(DescriptorPool&& other) noexcept;
        DescriptorPool& operator=(DescriptorPool&& other) noexcept;

        void addPoolSize(VkDescriptorType type, uint32_t count);
        void create(uint32_t maxSets);

        VkDescriptorSet allocate(VkDescriptorSetLayout layout);
        std::vector<VkDescriptorSet> allocate(VkDescriptorSetLayout layout, uint32_t count);

        VkDescriptorPool handle() const { return m_pool; }

    private:
        const Device& m_device;
        VkDescriptorPool m_pool{ VK_NULL_HANDLE };
        std::vector<VkDescriptorPoolSize> m_poolSizes;

        void cleanup();
    };
} // namespace vkcommon

#endif // DESCRIPTOR_POOL_H