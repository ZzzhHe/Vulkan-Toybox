#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <vulkan/vulkan.h>

namespace vkcommon {
    class Device;

    class Semaphore {
    public:
        explicit Semaphore(const Device& device);
        ~Semaphore();

        // Disable copying
        Semaphore(const Semaphore&) = delete;
        Semaphore& operator=(const Semaphore&) = delete;

        // Enable moving
        Semaphore(Semaphore&& other) noexcept;
        Semaphore& operator=(Semaphore&& other) noexcept;

        VkSemaphore handle() const { return m_semaphore; }

    private:
        void cleanup();

        const Device& m_device;
        VkSemaphore m_semaphore{ VK_NULL_HANDLE };
    };
} // namespace vkcommon

#endif // SEMAPHORE_H
