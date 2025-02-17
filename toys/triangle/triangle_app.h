#ifndef TRIANGLE_APP_H
#define TRIANGLE_APP_H

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
#include "resources/memory/memory_allocator.h"
#include "sync/frame_manager.h"
#include "resources/descriptors/descriptor_set_layout.h"
#include "resources/images/color_image.h"
#include "resources/images/depth_buffer.h"

#include <memory>
#include <vector>

class TriangleApp {
public:
    TriangleApp() = default;
    ~TriangleApp() = default;

    void run();

private: 
    static const int MAX_FRAMES_IN_FLIGHT = 2;

    void initVulkan();
    void mainLoop();
    void createVertexBuffer();
    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void drawFrame();

    vkcommon::Window m_window;
    vkcommon::Instance m_instance;
    vkcommon::Surface m_surface{ m_instance, m_window };
    vkcommon::PhysicalDevice m_physicalDevice{ m_instance, m_surface };
    vkcommon::Device m_device{ m_physicalDevice };
    vkcommon::MemoryAllocator m_allocator{ m_physicalDevice, m_device };
    vkcommon::SwapChain m_swapChain{ m_window, m_surface, m_physicalDevice, m_device };
    vkcommon::DescriptorSetLayout m_descriptorSetLayout{ m_device };
    std::unique_ptr<vkcommon::GraphicsPipeline> m_pipeline;
    vkcommon::CommandPool m_commandPool{ m_physicalDevice, m_device };
    std::vector<VkCommandBuffer> m_commandBuffers;
    vkcommon::VertexBuffer m_vertexBuffer{ m_device, m_allocator };

    vkcommon::ColorImage m_colorImage{ m_device, m_allocator };
    vkcommon::DepthBuffer m_depthBuffer{ m_device, m_allocator };

    vkcommon::FrameManager m_frameManager{ m_device, MAX_FRAMES_IN_FLIGHT };
};

#endif // TRIANGLE_APP_H
