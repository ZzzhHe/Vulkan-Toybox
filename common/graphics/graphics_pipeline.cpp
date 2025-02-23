#include "graphics_pipeline.h"

#include "core/device.h"
#include "graphics/shader_module.h"
#include "graphics/render_pass.h"
#include "graphics/pipeline_builder.h"
#include "graphics/swap_chain.h"
#include "resources/buffers/vertex_buffer.h"

#include <fstream>

namespace vkcommon
{

    GraphicsPipeline::GraphicsPipeline(
        const Device& device,
        const SwapChain& swapChain,
        const std::vector<VkDescriptorSetLayout>& descriptorLayout,
        const std::filesystem::path& vertPath,
        const std::filesystem::path& fragPath,
        const std::filesystem::path& geomPath)
        : m_deviceRef(device), m_swapChainRef(swapChain), m_renderPass(device, swapChain)
    {
        // Create shader modules
        std::vector<ShaderModule> shaderModules;
        shaderModules.reserve(3); // 0 : Vertex, 1 : Fragment, and 2 : Geometry shaders

        shaderModules.emplace_back(device, vertPath);
        shaderModules.emplace_back(device, fragPath);

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {
            {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_VERTEX_BIT, shaderModules[0], "main"},
            {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_FRAGMENT_BIT, shaderModules[1], "main"}
        };

        if (!geomPath.empty())
        {
            shaderModules.emplace_back(device, geomPath);
            shaderStages.push_back(
                { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_GEOMETRY_BIT, shaderModules[2], "main"}
            );
        }

        // Create pipeline layout
        createPipelineLayout(descriptorLayout);

        // Set up pipeline builder
        PipelineBuilder builder(device);
        builder
            .setShaderStages(shaderStages)
            .setVertexInput(Vertex::getBindingDescription(), Vertex::getAttributeDescriptions())
            .setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .setViewport()
            .setRasterizer()
            .setMultisampling()
            .setDepthStencil()
            .setColorBlending()
            .setDynamicState(m_dynamicState)
            .setPipelineLayout(m_pipelineLayout);

        // Build the pipeline
        m_graphicsPipeline = builder.build(m_renderPass);
    }

    GraphicsPipeline::~GraphicsPipeline()
    {
        vkDestroyPipeline(m_deviceRef.handle(), m_graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(m_deviceRef.handle(), m_pipelineLayout, nullptr);
    }

    void GraphicsPipeline::createPipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorLayout)
    {
        VkPipelineLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorLayout.size());
        layoutInfo.pSetLayouts = descriptorLayout.data();

        if (vkCreatePipelineLayout(m_deviceRef.handle(), &layoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }

    void GraphicsPipeline::bind(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint) const
    {
        vkCmdBindPipeline(commandBuffer, bindPoint, m_graphicsPipeline);
    }

    void GraphicsPipeline::setViewportState(VkCommandBuffer commandBuffer, const VkExtent2D& extent)
    {
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(extent.width);
        viewport.height = static_cast<float>(extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = extent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

} // namespace vkcommon