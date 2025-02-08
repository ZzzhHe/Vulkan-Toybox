#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace vkcommon
{
    class Device;
    class RenderPass;

    class PipelineBuilder
    {
    public:
        explicit PipelineBuilder(const PhysicalDevice& physicalDevice, const Device& device);

        PipelineBuilder& setVertexInput(const VkVertexInputBindingDescription& binding,
            const std::array<VkVertexInputAttributeDescription, 3>& attributes);
        PipelineBuilder& setShaderStages(const std::vector<VkPipelineShaderStageCreateInfo>& stages);
        PipelineBuilder& setInputAssembly(VkPrimitiveTopology topology);
        PipelineBuilder& setViewport(const VkExtent2D& extent);
        PipelineBuilder& setRasterizer(VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL);
        PipelineBuilder& setMultisampling();
        PipelineBuilder& setDepthStencil();
        PipelineBuilder& setColorBlending();
        PipelineBuilder& setDynamicState(const std::vector<VkDynamicState>& dynamicStates);
        PipelineBuilder& setPipelineLayout(VkPipelineLayout layout);

        VkPipeline build(const RenderPass& renderPass) const;

    private:
        const Device& m_device;
        const PhysicalDevice& m_physicalDevice;

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        VkPipelineViewportStateCreateInfo viewportState{};
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        VkPipelineMultisampleStateCreateInfo multisampling{};
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        VkPipelineDynamicStateCreateInfo dynamicState{};
        VkPipelineLayout pipelineLayout{};
    };

} // namespace vkcommon