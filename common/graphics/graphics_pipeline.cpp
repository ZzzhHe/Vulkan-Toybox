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
        const VkDescriptorSetLayout& descriptorLayout,
        const std::filesystem::path& vertPath,
        const std::filesystem::path& fragPath)
        : m_deviceRef(device), m_swapChainRef(swapChain), m_renderPass(device, swapChain)
    {
        // Create shader modules
        ShaderModule vertShader(m_deviceRef, vertPath);
        ShaderModule fragShader(m_deviceRef, fragPath);

        // Create pipeline layout
        createPipelineLayout(descriptorLayout);

        // Set up pipeline builder
        PipelineBuilder builder(device);
        builder
            .setShaderStages({ {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0,
                               VK_SHADER_STAGE_VERTEX_BIT, vertShader, "main"},
                              {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0,
                               VK_SHADER_STAGE_FRAGMENT_BIT, fragShader, "main"} })
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

    void GraphicsPipeline::createPipelineLayout(const VkDescriptorSetLayout& descriptorLayout)
    {
        VkPipelineLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.setLayoutCount = 1;
        layoutInfo.pSetLayouts = &descriptorLayout;

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