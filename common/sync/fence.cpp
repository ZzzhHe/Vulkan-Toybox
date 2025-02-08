#include "fence.h"

#include "core/device.h"

#include <stdexcept>

namespace vkcommon {
    Fence::Fence(const Device& device, bool signaled)
        : m_device(device) {
        VkFenceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        createInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

        if (vkCreateFence(m_device.handle(), &createInfo, nullptr, &m_fence) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create fence!");
        }
    }

    Fence::~Fence() {
        cleanup();
    }

    Fence::Fence(Fence&& other) noexcept
        : m_device(other.m_device)
        , m_fence(other.m_fence) {
        other.m_fence = VK_NULL_HANDLE;
    }

    Fence& Fence::operator=(Fence&& other) noexcept {
        if (this != &other) {
            cleanup();
            m_fence = other.m_fence;
            other.m_fence = VK_NULL_HANDLE;
        }
        return *this;
    }

    void Fence::wait(uint64_t timeout) const {
        if (vkWaitForFences(m_device.handle(), 1, &m_fence, VK_TRUE, timeout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to wait for fence!");
        }
    }

    void Fence::reset() const {
        if (vkResetFences(m_device.handle(), 1, &m_fence) != VK_SUCCESS) {
            throw std::runtime_error("Failed to reset fence!");
        }
    }

    void Fence::cleanup() {
        if (m_fence != VK_NULL_HANDLE) {
            vkDestroyFence(m_device.handle(), m_fence, nullptr);
            m_fence = VK_NULL_HANDLE;
        }
    }

}