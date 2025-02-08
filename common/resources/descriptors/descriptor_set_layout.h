#ifndef DESCRIPTOR_SET_LAYOUT_H
#define DESCRIPTOR_SET_LAYOUT_H

#include <vulkan/vulkan.h>
#include <vector>

namespace vkcommon {

    class Device;

    class DescriptorSetLayout {
    public:
        explicit DescriptorSetLayout(const Device& device);
        ~DescriptorSetLayout();

        // Disable copying
        DescriptorSetLayout(const DescriptorSetLayout&) = delete;
        DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

        // Enable moving
        DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;
        DescriptorSetLayout& operator=(DescriptorSetLayout&& other) noexcept;

        void addBinding(uint32_t binding,
            VkDescriptorType type,
            VkShaderStageFlags stageFlags,
            uint32_t count = 1);
        void create();

        VkDescriptorSetLayout handle() const { return m_layout; }

    private:
        const Device& m_device;
        VkDescriptorSetLayout m_layout{ VK_NULL_HANDLE };
        std::vector<VkDescriptorSetLayoutBinding> m_bindings;

        void cleanup();
    };
} // namespace vkcommon

#endif // DESCRIPTOR_SET_LAYOUT_H
