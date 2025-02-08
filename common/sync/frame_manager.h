#ifndef FRAME_MANAGER_H
#define FRAME_MANAGER_H

#include <vulkan/vulkan.h>

#include <vector>

#include "sync/frame_sync.h"

namespace vkcommon {

    class Device;

    class FrameManager {
    public:
        FrameManager(const Device& device, uint32_t maxFramesInFlight);
        ~FrameManager() = default;

        // Disable copying
        FrameManager(const FrameManager&) = delete;
        FrameManager& operator=(const FrameManager&) = delete;

        // Enable moving
        FrameManager(FrameManager&& other) noexcept;
        FrameManager& operator=(FrameManager&& other) noexcept;

        void waitForFence() const;
        void resetFence() const;
        void nextFrame() { m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight; }

        uint32_t currentFrame() const { return m_currentFrame; }
        const FrameSync& getCurrentSync() const { return m_framesyncs[m_currentFrame]; }

    private:
        std::vector<FrameSync> m_framesyncs;
        uint32_t m_currentFrame{ 0 };
        uint32_t m_maxFramesInFlight;

        const Device& m_deviceRef;
    };

} // namespace vkcommon

#endif // FRAME_MANAGER_H