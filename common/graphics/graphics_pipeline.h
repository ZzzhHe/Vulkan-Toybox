#ifndef GRAPHICS_PIPELINE_H
#define GRAPHICS_PIPELINE_H

#include <vector>
#include <string>

#include <vulkan/vulkan.h>
#include <filesystem>

#include "graphics/render_pass.h"

namespace vkcommon
{

    class Device;
    class SwapChain;
    class RenderPass;

    class GraphicsPipeline
    {
    public:
        GraphicsPipeline(
            const Device& device,
            const SwapChain& swapChain,
            const VkDescriptorSetLayout& descriptorLayout,
            const std::filesystem::path& vertPath,
            const std::filesystem::path& fragPath);

        ~GraphicsPipeline();

        void bind(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint) const;

        void setViewportState(VkCommandBuffer commandBuffer, const VkExtent2D& extent);

        VkPipeline handle() const { return m_graphicsPipeline; }
        VkPipelineLayout layout() const { return m_pipelineLayout; }
        const RenderPass& renderPass() const { return m_renderPass; }

    private:
        void createPipelineLayout(const VkDescriptorSetLayout& descriptorLayout);

        const std::vector<VkDynamicState> m_dynamicState = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

        RenderPass m_renderPass;

        VkPipelineLayout m_pipelineLayout;
        VkPipeline m_graphicsPipeline;

        const Device& m_deviceRef;
        const SwapChain& m_swapChainRef;
    };

} // namespace vkcommon
#endif // GRAPHICS_PIPELINE_H
