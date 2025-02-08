#include "descriptor_pool.h"

#include "core/device.h"

#include <stdexcept>

namespace vkcommon {
    DescriptorPool::DescriptorPool(const Device& device)
        : m_device(device) {
    }

    DescriptorPool::~DescriptorPool() {
        cleanup();
    }

    DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
        : m_device(other.m_device)
        , m_pool(other.m_pool)
        , m_poolSizes(std::move(other.m_poolSizes)) {
        other.m_pool = VK_NULL_HANDLE;
    }

    DescriptorPool& DescriptorPool::operator=(DescriptorPool&& other) noexcept {
        if (this != &other) {
            cleanup();
            m_pool = other.m_pool;
            m_poolSizes = std::move(other.m_poolSizes);
            other.m_pool = VK_NULL_HANDLE;
        }
        return *this;
    }

    void DescriptorPool::addPoolSize(VkDescriptorType type, uint32_t count) {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = type;
        poolSize.descriptorCount = count;
        m_poolSizes.push_back(poolSize);
    }

    void DescriptorPool::create(uint32_t maxSets) {
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(m_poolSizes.size());
        poolInfo.pPoolSizes = m_poolSizes.data();
        poolInfo.maxSets = maxSets;
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;  // Optional: allows freeing individual sets

        if (vkCreateDescriptorPool(m_device.handle(), &poolInfo, nullptr, &m_pool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool");
        }
    }

    VkDescriptorSet DescriptorPool::allocate(VkDescriptorSetLayout layout) {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_pool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;

        VkDescriptorSet descriptorSet;
        if (vkAllocateDescriptorSets(m_device.handle(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor set");
        }

        return descriptorSet;
    }

    std::vector<VkDescriptorSet> DescriptorPool::allocate(VkDescriptorSetLayout layout, uint32_t count) {
        std::vector<VkDescriptorSetLayout> layouts(count, layout);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_pool;
        allocInfo.descriptorSetCount = count;
        allocInfo.pSetLayouts = layouts.data();

        std::vector<VkDescriptorSet> descriptorSets(count);
        if (vkAllocateDescriptorSets(m_device.handle(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets");
        }

        return descriptorSets;
    }

    void DescriptorPool::cleanup() {
        if (m_pool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(m_device.handle(), m_pool, nullptr);
            m_pool = VK_NULL_HANDLE;
        }
    }
}