#include "cube_exploder.h"

void CubeExploder::run() {
    initVulkan();
    mainLoop();
}

void CubeExploder::initVulkan() {
    m_descriptorSetLayout.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    m_descriptorSetLayout.create();

    m_uniformBuffer.create(sizeof(UniformBufferObject), MAX_FRAMES_IN_FLIGHT);

    m_descriptorPool.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT);
    m_descriptorPool.create(MAX_FRAMES_IN_FLIGHT);

    m_descriptorSets = m_descriptorPool.allocate(m_descriptorSetLayout.handle(), MAX_FRAMES_IN_FLIGHT);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkcommon::DescriptorWriter descriptorWriter{ m_descriptorSets[i] };
        descriptorWriter.writeBuffer(
            0,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            m_uniformBuffer.buffer(i),
            sizeof(UniformBufferObject)
        );
        descriptorWriter.update(m_device);
    }

    // Create graphics pipeline
    m_pipeline = std::make_unique<vkcommon::GraphicsPipeline>(
        m_device,
        m_swapChain,
        m_descriptorSetLayout.handle(),
        "shaders/cube.vert.spv",
        "shaders/cube.frag.spv"
    );

    // Create color image
    m_colorImage.create(m_swapChain);
    m_depthBuffer.create(m_swapChain);

    m_swapChain.createFrameBuffers(
        m_pipeline->renderPass(),
        m_colorImage.imageView(),
        m_depthBuffer.imageView());

    createVertexBuffer();
    createCommandBuffers();
}

void CubeExploder::createVertexBuffer() {
    // Define cube vertices (8 corners)
    const float size = 0.5f;  // Half-size of the cube
    std::vector<vkcommon::Vertex> vertices = {
        // Front face
        {{-size, -size, size}, {0, 0, 1}, {0, 0}},  // Front bottom-left
        {{size, -size, size}, {0, 0, 1}, {1, 0}},   // Front bottom-right
        {{size, size, size}, {0, 0, 1}, {1, 1}},    // Front top-right
        {{-size, size, size}, {0, 0, 1}, {0, 1}},   // Front top-left

        // Back face
        {{-size, -size, -size}, {0, 0, -1}, {1, 0}}, // Back bottom-left
        {{size, -size, -size}, {0, 0, -1}, {0, 0}},  // Back bottom-right
        {{size, size, -size}, {0, 0, -1}, {0, 1}},   // Back top-right
        {{-size, size, -size}, {0, 0, -1}, {1, 1}},  // Back top-left
    };

    // Define indices for all faces
    std::vector<uint32_t> indices = {
        // Front face
        0, 1, 2,
        2, 3, 0,
        // Right face
        1, 5, 6,
        6, 2, 1,
        // Back face
        5, 4, 7,
        7, 6, 5,
        // Left face
        4, 0, 3,
        3, 7, 4,
        // Top face
        3, 2, 6,
        6, 7, 3,
        // Bottom face
        4, 5, 1,
        1, 0, 4
    };

    m_vertexBuffer.createVertexBuffer(vertices, m_commandPool);
    m_vertexBuffer.createIndexBuffer(indices, m_commandPool);
}

void CubeExploder::createCommandBuffers() {
    m_commandBuffers = m_commandPool.allocateBuffers(MAX_FRAMES_IN_FLIGHT);
}

void CubeExploder::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    // Begin command buffer recording
    m_commandPool.beginCommandBuffer(commandBuffer, 0);

    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = { {0.9f, 0.9f, 0.9f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    // Begin render pass
    m_pipeline->renderPass().begin(
        commandBuffer,
        m_swapChain.swapChainFramebuffer(imageIndex),
        m_swapChain.swapChainExtent(),
        clearValues
    );

    // Bind pipeline
    m_pipeline->bind(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS);
    m_pipeline->setViewportState(commandBuffer, m_swapChain.swapChainExtent());

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline->layout(),
        0,  // First set
        1,  // One set
        &m_descriptorSets[m_frameManager.currentFrame()],
        0,
        nullptr
    );

    // Bind vertex buffer
    m_vertexBuffer.bindVertexBuffer(commandBuffer, 0);
    m_vertexBuffer.bindIndexBuffer(commandBuffer, VK_INDEX_TYPE_UINT32);

    // Draw
    vkCmdDrawIndexed(commandBuffer, 36, 1, 0, 0, 0);

    // End render pass
    m_pipeline->renderPass().end(commandBuffer);

    // End command buffer recording
    m_commandPool.endCommandBuffer(commandBuffer);
}

void CubeExploder::updateUniformBuffer(uint32_t currentImage)
{
    UniformBufferObject ubo{};

    // Model matrix
    ubo.model = glm::rotate(
        glm::mat4(1.0f),
        glm::radians(90.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    // View matrix
    ubo.view = glm::lookAt(
        glm::vec3(2.0f, 2.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // Projective matrix
    ubo.proj = glm::perspective(
        glm::radians(45.0f),                                    // 45 degree FOV
        static_cast<float>(m_swapChain.swapChainExtent().width) /
        static_cast<float>(m_swapChain.swapChainExtent().height), // Aspect ratio
        0.1f,                                                   // Near plane
        10.0f                                                   // Far plane
    );

    // Vulkan's Y coordinate is inverted compared to OpenGL
    ubo.proj[1][1] *= -1;

    m_uniformBuffer.updateData(currentImage, &ubo, sizeof(ubo));
}

void CubeExploder::drawFrame() {
    m_frameManager.waitForFence();

    updateUniformBuffer(m_frameManager.currentFrame());

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        m_device.handle(),
        m_swapChain.handle(),
        UINT64_MAX,
        m_frameManager.getCurrentSync().imageAvailable(),
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    m_frameManager.resetFence();

    vkResetCommandBuffer(m_commandBuffers[m_frameManager.currentFrame()], 0);
    recordCommandBuffer(m_commandBuffers[m_frameManager.currentFrame()], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_frameManager.getCurrentSync().imageAvailable() };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[m_frameManager.currentFrame()];

    VkSemaphore signalSemaphores[] = { m_frameManager.getCurrentSync().renderFinished() };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_device.graphicsQueue(), 1, &submitInfo, m_frameManager.getCurrentSync().fence()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { m_swapChain.handle() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(m_device.presentQueue(), &presentInfo);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    m_frameManager.nextFrame();
}

void CubeExploder::mainLoop() {
    while (!m_window.shouldClose()) {
        m_window.pollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(m_device.handle());
}