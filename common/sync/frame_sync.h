#ifndef FRAME_SYNC_H
#define FRAME_SYNC_H

#include <vulkan/vulkan.h>

#include "semaphore.h"
#include "fence.h"

namespace vkcommon {
    class Device;

    class FrameSync {
    public:
        FrameSync(const Device& device, bool fenceSignaled = true);
        ~FrameSync() = default;

        // Disable copying
        FrameSync(const FrameSync&) = delete;
        FrameSync& operator=(const FrameSync&) = delete;

        // Enable moving
        FrameSync(FrameSync&& other) noexcept;
        FrameSync& operator=(FrameSync&& other) noexcept;

        void waitForFence(uint64_t timeout = UINT64_MAX) const { m_inFlightFence.wait(timeout); }
        void resetFence() const { m_inFlightFence.reset(); }

        VkSemaphore imageAvailable() const { return m_imageAvailable.handle(); }
        VkSemaphore renderFinished() const { return m_renderFinished.handle(); }
        VkFence fence() const { return m_inFlightFence.handle(); }

    private:
        Semaphore m_imageAvailable;
        Semaphore m_renderFinished;
        Fence m_inFlightFence;

        const Device& m_deviceRef;
    };
} // namespace vkcommon

#endif // FRAME_SYNC_H