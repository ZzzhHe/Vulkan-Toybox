#include "frame_manager.h"

#include "core/device.h"

namespace vkcommon {
    FrameManager::FrameManager(const Device& device, uint32_t maxFramesInFlight)
        : m_deviceRef(device), m_maxFramesInFlight(maxFramesInFlight) {
        m_framesyncs.reserve(maxFramesInFlight);
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            m_framesyncs.emplace_back(device);
        }
    }

    FrameManager::FrameManager(FrameManager&& other) noexcept
        : m_deviceRef(other.m_deviceRef)
        , m_framesyncs(std::move(other.m_framesyncs))
        , m_currentFrame(other.m_currentFrame)
        , m_maxFramesInFlight(other.m_maxFramesInFlight) {
        other.m_currentFrame = 0;
        other.m_maxFramesInFlight = 0;
    }

    FrameManager& FrameManager::operator=(FrameManager&& other) noexcept {
        if (this != &other) {
            m_framesyncs = std::move(other.m_framesyncs);
            m_currentFrame = other.m_currentFrame;
            m_maxFramesInFlight = other.m_maxFramesInFlight;

            other.m_currentFrame = 0;
            other.m_maxFramesInFlight = 0;
        }
        return *this;
    }

    void FrameManager::waitForFence() const {
        m_framesyncs[m_currentFrame].waitForFence();
    }

    void FrameManager::resetFence() const {
        m_framesyncs[m_currentFrame].resetFence();
    }
} // namespace vkcommon