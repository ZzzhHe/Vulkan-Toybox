#include "cube_app.h"

void CubeApp::run() {
    initVulkan();
    mainLoop();
}

void CubeApp::initVulkan() {
    m_descriptorSetLayout.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    m_descriptorSetLayout.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    m_descriptorSetLayout.create();

    m_texture.loadFromFile("D:\\Projects\\Vulkan-Toybox\\toys\\cube\\dice_texture.png", m_commandPool);
    m_texture.createSampler();

    m_uniformBuffer.create(sizeof(UniformBufferObject), MAX_FRAMES_IN_FLIGHT);

    m_descriptorPool.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT);
    m_descriptorPool.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT);
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
        descriptorWriter.writeImage(
            1,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            m_texture.imageView(),
            m_texture.sampler()
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

void CubeApp::createVertexBuffer() {
    // Define cube vertices (8 corners)
    const float size = 0.5f;  // Half-size of the cube
    std::vector<vkcommon::Vertex> vertices = {
        // Front face (2)
        {{-size, -size,  size}, {0.0f, 0.0f, 1.0f}, {0.25f, 0.5f}},    // Bottom-left
        {{ size, -size,  size}, {0.0f, 0.0f, 1.0f}, {0.5f, 0.5f}},     // Bottom-right
        {{ size,  size,  size}, {0.0f, 0.0f, 1.0f}, {0.5f, 0.75f}},    // Top-right
        {{-size,  size,  size}, {0.0f, 0.0f, 1.0f}, {0.25f, 0.75f}},   // Top-left

        // Back face (1)
        {{-size, -size, -size}, {0.0f, 0.0f, -1.0f}, {0.5f, 0.5f}},    // Bottom-left
        {{ size, -size, -size}, {0.0f, 0.0f, -1.0f}, {0.75f, 0.5f}},   // Bottom-right
        {{ size,  size, -size}, {0.0f, 0.0f, -1.0f}, {0.75f, 0.75f}},  // Top-right
        {{-size,  size, -size}, {0.0f, 0.0f, -1.0f}, {0.5f, 0.75f}},   // Top-left

        // Top face (6)
        {{-size,  size, -size}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.25f}},    // Back-left
        {{ size,  size, -size}, {0.0f, 1.0f, 0.0f}, {0.75f, 0.25f}},   // Back-right
        {{ size,  size,  size}, {0.0f, 1.0f, 0.0f}, {0.75f, 0.5f}},    // Front-right
        {{-size,  size,  size}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f}},     // Front-left

        // Bottom face (5)
        {{-size, -size, -size}, {0.0f, -1.0f, 0.0f}, {0.5f, 0.75f}},   // Back-left
        {{ size, -size, -size}, {0.0f, -1.0f, 0.0f}, {0.75f, 0.75f}},  // Back-right
        {{ size, -size,  size}, {0.0f, -1.0f, 0.0f}, {0.75f, 1.0f}},   // Front-right
        {{-size, -size,  size}, {0.0f, -1.0f, 0.0f}, {0.5f, 1.0f}},    // Front-left

        // Right face (4)
        {{ size, -size, -size}, {1.0f, 0.0f, 0.0f}, {0.75f, 0.5f}},    // Bottom-back
        {{ size,  size, -size}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.5f}},     // Top-back
        {{ size,  size,  size}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.75f}},    // Top-front
        {{ size, -size,  size}, {1.0f, 0.0f, 0.0f}, {0.75f, 0.75f}},   // Bottom-front

        // Left face (3)
        {{-size, -size, -size}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.5f}},    // Bottom-back
        {{-size,  size, -size}, {-1.0f, 0.0f, 0.0f}, {0.25f, 0.5f}},   // Top-back
        {{-size,  size,  size}, {-1.0f, 0.0f, 0.0f}, {0.25f, 0.75f}},  // Top-front
        {{-size, -size,  size}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.75f}}    // Bottom-front
    };

    // Indices remain the same as before
    std::vector<uint32_t> indices = {
        // Front face
        0,  1,  2,  2,  3,  0,
        // Back face
        4,  5,  6,  6,  7,  4,
        // Top face
        8,  9,  10, 10, 11, 8,
        // Bottom face
        12, 13, 14, 14, 15, 12,
        // Right face
        16, 17, 18, 18, 19, 16,
        // Left face
        20, 21, 22, 22, 23, 20
    };


    m_vertexBuffer.createVertexBuffer(vertices, m_commandPool);
    m_vertexBuffer.createIndexBuffer(indices, m_commandPool);
}

void CubeApp::createCommandBuffers() {
    m_commandBuffers = m_commandPool.allocateBuffers(MAX_FRAMES_IN_FLIGHT);
}

void CubeApp::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
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

void CubeApp::updateUniformBuffer(uint32_t currentImage)
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

void CubeApp::drawFrame() {
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

void CubeApp::mainLoop() {
    while (!m_window.shouldClose()) {
        m_window.pollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(m_device.handle());
}