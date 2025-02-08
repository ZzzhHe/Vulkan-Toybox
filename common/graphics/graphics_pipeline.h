#ifndef GRAPHICS_PIPELINE_H
#define GRAPHICS_PIPELINE_H

#include <vector>
#include <string>

#include <vulkan/vulkan.h>
#include <filesystem>

namespace vkcommon
{

    class Device;
    class SwapChain;

    class GraphicsPipeline
    {
    public:
        GraphicsPipeline(
            const PhysicalDevice& physicalDevice,
            const Device& device,
            const SwapChain& swapChain,
            const VkDescriptorSetLayout& descriptorLayout,
            const std::filesystem::path& vertPath,
            const std::filesystem::path& fragPath);

        ~GraphicsPipeline();

        VkPipeline handle() const { return m_graphicsPipeline; }
        VkPipelineLayout layout() const { return m_pipelineLayout; }
        VkRenderPass renderPass() const { return m_renderPass; }

    private:
        void createPipelineLayout(const VkDescriptorSetLayout& descriptorLayout);

        VkPipelineLayout m_pipelineLayout;
        VkPipeline m_graphicsPipeline;
        RenderPass m_renderPass;

        const Device& m_deviceRef;
        const SwapChain& m_swapChainRef;
    };

} // namespace vkcommon
#endif // GRAPHICS_PIPELINE_H
