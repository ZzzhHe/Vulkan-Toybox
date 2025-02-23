#include "model_app.h"

void ModelApp::run() {
    initVulkan();
    mainLoop();
}

void ModelApp::initVulkan() {
    createDescriptorSetLayout();
    createDescriptorPool();

    // Load model
    m_model = std::make_unique<vkcommon::Model>(m_device, m_allocator);
    m_model->loadFromFile(
        "D:\\Projects\\Vulkan-Toybox\\toys\\model\\nuka_cup\\nuka_cup.obj",
        m_textureLib,
        m_commandPool
    );

    createGlobalDescriptorSets();
    m_model->createDescriptor(
        m_descriptorPool,
        *vkcommon::Material::getDescriptorSetLayout(),
        MAX_FRAMES_IN_FLIGHT);

    std::vector<VkDescriptorSetLayout> layouts = {
        m_globalDescriptorSetLayout.handle(),           // set = 0
        vkcommon::Material::getDescriptorSetLayout()->handle()       // set = 1
    };
    
    // Create graphics pipeline
    m_pipeline = std::make_unique<vkcommon::GraphicsPipeline>(
        m_device,
        m_swapChain,
        layouts,
        "shaders/model.vert.spv",
        "shaders/model.frag.spv"
    );

    // Create color image
    m_colorImage.create(m_swapChain);
    m_depthBuffer.create(m_swapChain);

    m_swapChain.createFrameBuffers(
        m_pipeline->renderPass(),
        m_colorImage.imageView(),
        m_depthBuffer.imageView());

    createCommandBuffers();
}

void ModelApp::createCommandBuffers() {
    m_commandBuffers = m_commandPool.allocateBuffers(MAX_FRAMES_IN_FLIGHT);
}

void ModelApp::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
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

    // Bind global descriptor set
    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline->layout(),
        0,  // First set
        1,  // One set
        &m_globalDescriptorSets[m_frameManager.currentFrame()],
        0,
        nullptr
    );
    // model descriptor set has been handled fo each mesh in model class (mesh->draw())
    m_model->draw(commandBuffer, m_frameManager.currentFrame(), m_pipeline->layout());

    // End render pass
    m_pipeline->renderPass().end(commandBuffer);

    // End command buffer recording
    m_commandPool.endCommandBuffer(commandBuffer);
}

void ModelApp::updateGlobalUniformBuffer(uint32_t currentImage)
{
    GlobalUniformBufferObject ubo{};

    // Model matrix
    ubo.model = glm::rotate(
        glm::mat4(1.0f),
        glm::radians(45.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
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

    m_globalUBO.updateData(currentImage, &ubo, sizeof(ubo));
}

void ModelApp::drawFrame() {
    m_frameManager.waitForFence();

    updateGlobalUniformBuffer(m_frameManager.currentFrame());
    m_model->updateProperties(m_frameManager.currentFrame());

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

void ModelApp::mainLoop() {
    while (!m_window.shouldClose()) {
        m_window.pollEvents();
        drawFrame();
    }
    
    // destroy the static material descriptor layout
    vkcommon::Material::destroyDescriptorSetLayout();
    vkDeviceWaitIdle(m_device.handle());
}

void ModelApp::createDescriptorSetLayout()
{
    m_globalDescriptorSetLayout.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    m_globalDescriptorSetLayout.create();

    vkcommon::Material::createDescriptorSetLayout(m_device);

}

void ModelApp::createDescriptorPool()
{
    const uint32_t maxMaterials = 100;  // Adjust based on your needs
    const uint32_t maxTextures = maxMaterials * 3;  // Assume up to 3 textures per material
    
    // global uniform buffer
    m_descriptorPool.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT);
    // material properties
    m_descriptorPool.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT * maxMaterials);
    // material textures
    m_descriptorPool.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT * maxTextures);
    m_descriptorPool.create(MAX_FRAMES_IN_FLIGHT * (1 + maxMaterials));
}

void ModelApp::createGlobalDescriptorSets()
{
    // model's descriptor has been handled in model class

    m_globalUBO.create(sizeof(GlobalUniformBufferObject), MAX_FRAMES_IN_FLIGHT);

    m_globalDescriptorSets = m_descriptorPool.allocate(m_globalDescriptorSetLayout.handle(), MAX_FRAMES_IN_FLIGHT);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkcommon::DescriptorWriter descriptorWriter{ m_globalDescriptorSets[i] };
        descriptorWriter.writeBuffer(
            0,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            m_globalUBO.buffer(i),
            sizeof(GlobalUniformBufferObject)
        );

        descriptorWriter.update(m_device);
    }

}

void ModelApp::createModelDescriptorSets()
{

}
