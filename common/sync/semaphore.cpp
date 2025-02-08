#include "semaphore.h"

#include "core/device.h"

#include <stdexcept>

namespace vkcommon {

    // semaphore.cpp
    Semaphore::Semaphore(const Device& device)
        : m_device(device) {
        VkSemaphoreCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        if (vkCreateSemaphore(m_device.handle(), &createInfo, nullptr, &m_semaphore) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create semaphore!");
        }
    }

    Semaphore::~Semaphore() {
        cleanup();
    }

    void Semaphore::cleanup() {
        if (m_semaphore != VK_NULL_HANDLE) {
            vkDestroySemaphore(m_device.handle(), m_semaphore, nullptr);
            m_semaphore = VK_NULL_HANDLE;
        }
    }

    Semaphore::Semaphore(Semaphore&& other) noexcept
        : m_device(other.m_device)
        , m_semaphore(other.m_semaphore) {
        other.m_semaphore = VK_NULL_HANDLE;
    }

    Semaphore& Semaphore::operator=(Semaphore&& other) noexcept {
        if (this != &other) {
            cleanup();
            m_semaphore = other.m_semaphore;
            other.m_semaphore = VK_NULL_HANDLE;
        }
        return *this;
    }

} // namespace vkcommon