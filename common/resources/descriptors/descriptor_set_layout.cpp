#include "descriptor_set_layout.h"
#include "core/device.h"
#include <stdexcept>

namespace vkcommon {

    DescriptorSetLayout::DescriptorSetLayout(const Device& device)
        : m_device(device) {
    }

    DescriptorSetLayout::~DescriptorSetLayout() {
        cleanup();
    }

    DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept
        : m_device(other.m_device)
        , m_layout(other.m_layout)
        , m_bindings(std::move(other.m_bindings)) {
        other.m_layout = VK_NULL_HANDLE;
    }

    DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout&& other) noexcept {
        if (this != &other) {
            cleanup();
            m_layout = other.m_layout;
            m_bindings = std::move(other.m_bindings);
            other.m_layout = VK_NULL_HANDLE;
        }
        return *this;
    }

    void DescriptorSetLayout::addBinding(uint32_t binding,
        VkDescriptorType type,
        VkShaderStageFlags stageFlags,
        uint32_t count) {
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = type;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        layoutBinding.pImmutableSamplers = nullptr;

        m_bindings.push_back(layoutBinding);
    }

    void DescriptorSetLayout::create() {
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(m_bindings.size());
        layoutInfo.pBindings = m_bindings.data();

        if (vkCreateDescriptorSetLayout(m_device.handle(), &layoutInfo, nullptr, &m_layout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout");
        }
    }

    void DescriptorSetLayout::cleanup() {
        if (m_layout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(m_device.handle(), m_layout, nullptr);
            m_layout = VK_NULL_HANDLE;
        }
    }
} // namespace vkcommon