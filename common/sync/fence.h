#ifndef FENCE_H
#define FENCE_H

#include <vulkan/vulkan.h>

namespace vkcommon {

    class Device;

    class Fence {
    public:
        explicit Fence(const Device& device, bool signaled = true);
        ~Fence();

        // Disable copying
        Fence(const Fence&) = delete;
        Fence& operator=(const Fence&) = delete;
        
        // Enable moving
        Fence(Fence&& other) noexcept;
        Fence& operator=(Fence&& other) noexcept;

        void wait(uint64_t timeout = UINT64_MAX) const;
        void reset() const;

        VkFence handle() const { return m_fence; }

    private:
        void cleanup();

        const Device& m_device;
        VkFence m_fence{ VK_NULL_HANDLE };
    };
} // namespace vkcommon

#endif // FENCE_H