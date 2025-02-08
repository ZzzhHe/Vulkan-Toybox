#include "frame_sync.h"

#include "core/device.h"
#include <utility>

namespace vkcommon {
    FrameSync::FrameSync(const Device& device, bool fenceSignaled)
        : m_deviceRef(device)
        , m_imageAvailable(device)
        , m_renderFinished(device)
        , m_inFlightFence(device, fenceSignaled) {
    }

    FrameSync::FrameSync(FrameSync&& other) noexcept
        : m_deviceRef(other.m_deviceRef)
        , m_imageAvailable(std::move(other.m_imageAvailable))
        , m_renderFinished(std::move(other.m_renderFinished))
        , m_inFlightFence(std::move(other.m_inFlightFence)) {
    }

    FrameSync& FrameSync::operator=(FrameSync&& other) noexcept {
        if (this != &other) {
            m_imageAvailable = std::move(other.m_imageAvailable);
            m_renderFinished = std::move(other.m_renderFinished);
            m_inFlightFence = std::move(other.m_inFlightFence);
        }
        return *this;
    }
} // namespace vkcommon