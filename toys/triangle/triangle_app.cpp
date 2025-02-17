#include "triangle_app.h"

void TriangleApp::run() {
    initVulkan();
    mainLoop();
}

void TriangleApp::initVulkan() {
    // Create descriptor set layout (empty for basic triangle)
    m_descriptorSetLayout.create();

    // Create graphics pipeline
    m_pipeline = std::make_unique<vkcommon::GraphicsPipeline>(
        m_device,
        m_swapChain,
        m_descriptorSetLayout.handle(),
        "shaders/triangle.vert.spv",
        "shaders/triangle.frag.spv"
    );

    // Create color image
    m_colorImage.create(m_swapChain);
    m_depthBuffer.create(m_swapChain);

    m_swapChain.createFrameBuffers(m_pipeline->renderPass(), m_colorImage.imageView(), m_depthBuffer.imageView());
    createVertexBuffer();
    createCommandBuffers();
}

void TriangleApp::createVertexBuffer() {
    std::vector<vkcommon::Vertex> vertices = {
        {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.0f} , {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}
    };

    m_vertexBuffer.createVertexBuffer(vertices, m_commandPool);
}

void TriangleApp::createCommandBuffers() {
    m_commandBuffers = m_commandPool.allocateBuffers(MAX_FRAMES_IN_FLIGHT);
}

void TriangleApp::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    // Begin command buffer recording
    m_commandPool.beginCommandBuffer(commandBuffer, 0);

    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = { {1.0f, 1.0f, 1.0f, 1.0f} };
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

    // Bind vertex buffer
    m_vertexBuffer.bindVertexBuffer(commandBuffer, 0);

    // Draw
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    // End render pass
    m_pipeline->renderPass().end(commandBuffer);

    // End command buffer recording
    m_commandPool.endCommandBuffer(commandBuffer);
}

void TriangleApp::drawFrame() {
    m_frameManager.waitForFence();

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

void TriangleApp::mainLoop() {
    while (!m_window.shouldClose()) {
        m_window.pollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(m_device.handle());
}