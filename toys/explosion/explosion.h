#ifndef EXPLOSION_H
#define EXPLOSION_H

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
#include "sync/frame_manager.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <vector>

class Explosion {
public:
    Explosion() = default;
    ~Explosion() = default;

    void run();

private:
    static const int MAX_FRAMES_IN_FLIGHT = 2;

    struct UniformBufferObject
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
        float time;
    };

    void initVulkan();
    void mainLoop();
    void createVertexBuffer();
    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void updateUniformBuffer(uint32_t currentImage);
    void drawFrame();

    float m_time = 0.0f;

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

    // Pipeline and descriptor
    std::unique_ptr<vkcommon::GraphicsPipeline> m_pipeline;
    std::vector<VkDescriptorSet> m_descriptorSets;
    vkcommon::DescriptorSetLayout m_descriptorSetLayout{ m_device };
    vkcommon::DescriptorPool m_descriptorPool{ m_device };

    // Resources
    vkcommon::VertexBuffer m_vertexBuffer{ m_device, m_allocator };
    vkcommon::ColorImage m_colorImage{ m_device, m_allocator };
    vkcommon::DepthBuffer m_depthBuffer{ m_device, m_allocator };
    vkcommon::UniformBuffer m_uniformBuffer{ m_device, m_allocator };
    vkcommon::Texture m_texture{ m_device, m_allocator };
    vkcommon::Buffer m_explosionSSBOBuffer{ m_device, m_allocator };

    vkcommon::FrameManager m_frameManager{ m_device, MAX_FRAMES_IN_FLIGHT };
};

#endif // EXPLOSION_H
