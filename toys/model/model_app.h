#ifndef MODEL_APP_H
#define MODEL_APP_H

#include <vulkan/vulkan_core.h>

#include "core/window.h"
#include "core/instance.h"
#include "core/surface.h"
#include "core/physical_device.h"
#include "core/device.h"
#include "graphics/swap_chain.h"
#include "graphics/graphics_pipeline.h"
#include "graphics/command_pool.h"
#include "resources/buffers/vertex_buffer.h"
#include "resources/buffers/uniform_buffer.h"
#include "resources/descriptors/descriptor_set_layout.h"
#include "resources/descriptors/descriptor_pool.h"
#include "resources/descriptors/descriptor_writer.h"
#include "resources/images/color_image.h"
#include "resources/images/depth_buffer.h"
#include "resources/memory/memory_allocator.h"
#include "resources/images/texture.h"
#include "resources/model/texture_lib.h"
#include "resources/model/model.h"
#include "resources/model/material.h"
#include "sync/frame_manager.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <vector>

class ModelApp {
public:
    ModelApp() = default;
    ~ModelApp() = default;

    void run();

private:
    static const int MAX_FRAMES_IN_FLIGHT = 2;

    struct GlobalUniformBufferObject
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    void initVulkan();
    void mainLoop();
    void drawFrame();

    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createGlobalDescriptorSets();
    void createModelDescriptorSets();

    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void updateGlobalUniformBuffer(uint32_t currentImage);

    // Core Vulkan Objects
    vkcommon::Window m_window;
    vkcommon::Instance m_instance;
    vkcommon::Surface m_surface{ m_instance, m_window };
    vkcommon::PhysicalDevice m_physicalDevice{ m_instance, m_surface };
    vkcommon::Device m_device{ m_physicalDevice };
    vkcommon::MemoryAllocator m_allocator{ m_physicalDevice, m_device };
    vkcommon::SwapChain m_swapChain{ m_window, m_surface, m_physicalDevice, m_device };

    vkcommon::CommandPool m_commandPool{ m_physicalDevice, m_device };
    std::vector<VkCommandBuffer> m_commandBuffers;

    vkcommon::TextureLibrary m_textureLib{ m_device, m_allocator };

    // Pipeline and descriptor
    std::unique_ptr<vkcommon::GraphicsPipeline> m_pipeline;
    std::vector<VkDescriptorSet> m_globalDescriptorSets;
    vkcommon::DescriptorSetLayout m_globalDescriptorSetLayout{ m_device };
    vkcommon::DescriptorPool m_descriptorPool{ m_device };

    // Resources
    vkcommon::ColorImage m_colorImage{ m_device, m_allocator };
    vkcommon::DepthBuffer m_depthBuffer{ m_device, m_allocator };
    vkcommon::UniformBuffer m_globalUBO{ m_device, m_allocator };
    std::unique_ptr<vkcommon::Model> m_model;

    vkcommon::FrameManager m_frameManager{ m_device, MAX_FRAMES_IN_FLIGHT };
};

#endif // MODEL_APP_H
