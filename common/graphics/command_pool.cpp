#include "command_pool.h"

#include "core/device.h"
#include "core/physical_device.h"

#include <stdexcept>

namespace vkcommon
{
    CommandPool::CommandPool(const PhysicalDevice& physicalDevice, const Device& device)
        : m_deviceRef(device)
    {

        VkCommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = physicalDevice.queueFamilyIndices().graphicsFamily.value();

        if (vkCreateCommandPool(device.handle(), &commandPoolCreateInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create command pool!");
        }
    }

    CommandPool::~CommandPool()
    {
        vkDestroyCommandPool(m_deviceRef.handle(), m_commandPool, nullptr);
    }

    std::vector<VkCommandBuffer> CommandPool::allocateBuffers(uint32_t count, VkCommandBufferLevel level) const
    {
        std::vector<VkCommandBuffer> commandBuffers(count);
        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = m_commandPool;
        allocateInfo.level = level;
        allocateInfo.commandBufferCount = count;

        if (vkAllocateCommandBuffers(m_deviceRef.handle(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate command buffer!");
        }

        return commandBuffers;
    }

    VkCommandBuffer CommandPool::allocateSingleBuffer(VkCommandBufferLevel level) const
    {
        return allocateBuffers(1, level)[0];
    }

    void CommandPool::freeBuffers(const std::vector<VkCommandBuffer>& buffers) const
    {
        vkFreeCommandBuffers(m_deviceRef.handle(), m_commandPool, static_cast<uint32_t>(buffers.size()), buffers.data());
    }

    void CommandPool::freeSingleBuffer(VkCommandBuffer buffer) const
    {
        vkFreeCommandBuffers(m_deviceRef.handle(), m_commandPool, 1, &buffer);
    }

    VkCommandBuffer CommandPool::beginCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags flags) const
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = flags;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }

        return commandBuffer;
    }

    void CommandPool::endCommandBuffer(VkCommandBuffer commandBuffer) const
    {
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer!");
        }
    }

    VkCommandBuffer CommandPool::beginSingleTimeCommand() const
    {
        
        VkCommandBuffer commandBuffer = allocateSingleBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        beginCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        return commandBuffer;
    }

    void CommandPool::endSingleTimeCommand(VkCommandBuffer commandBuffer, VkQueue queue) const
    {
        endCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(queue);

        freeSingleBuffer(commandBuffer);
    }
}