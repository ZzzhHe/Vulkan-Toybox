#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include <vulkan/vulkan.h>

#include <vector>

namespace vkcommon
{
    class PhysicalDevice;
    class Device;
    class SwapChain;

    class RenderPass
    {
    public:
        RenderPass(const Device& device, const SwapChain& swapChain);
        ~RenderPass();

        RenderPass(const RenderPass&) = delete;
        RenderPass& operator=(const RenderPass&) = delete;

        void begin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent, const std::vector<VkClearValue>& clearValues) const;
        void end(VkCommandBuffer commandBuffer) const;

        operator VkRenderPass() const noexcept { return m_renderPass; }

    private:
        VkRenderPass m_renderPass;
        const Device& m_deviceRef;
        const SwapChain& m_swapChainRef;
    };
}

#endif // RENDER_PASS_H