#include "pipeline_builder.h"

#include "core/device.h"
#include "graphics/render_pass.h"

#include <array>
#include <stdexcept>

namespace vkcommon
{

    PipelineBuilder::PipelineBuilder(const Device& device) : m_device(device)
    {
        // Default initialization
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    }

    PipelineBuilder& PipelineBuilder::setVertexInput(
        const VkVertexInputBindingDescription& binding,
        const std::array<VkVertexInputAttributeDescription, 5>& attributes)
    {
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &binding;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributes.data();
        return *this;
    }

    PipelineBuilder& PipelineBuilder::setShaderStages(
        const std::vector<VkPipelineShaderStageCreateInfo>& stages)
    {
        shaderStages = stages;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::setInputAssembly(VkPrimitiveTopology topology)
    {
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = topology;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::setViewport()
    {
        viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;
        // Set to nullptr to use dynamic states
        viewportState.pScissors = nullptr;
        viewportState.pViewports = nullptr;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::setRasterizer(VkPolygonMode polygonMode)
    {
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE; // may be used for shadow mapping when VK_TRUE
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = polygonMode;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        // optional
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::setMultisampling()
    {
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_TRUE;
        multisampling.rasterizationSamples = m_device.msaaSamples();
        // Optional
        multisampling.minSampleShading = .2f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::setDepthStencil()
    {
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::setColorBlending()
    {
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        // optional
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        // Optional
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::setDynamicState(const std::vector<VkDynamicState>& dynamicStates)
    {
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();
        return *this;
    }

    PipelineBuilder& PipelineBuilder::setPipelineLayout(VkPipelineLayout layout)
    {
        pipelineLayout = layout;
        return *this;
    }

    VkPipeline PipelineBuilder::build(const RenderPass& renderPass) const
    {
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        VkPipeline pipeline;
        if (vkCreateGraphicsPipelines(m_device.handle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }
        return pipeline;
    }

} // namespace vkcommon