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

    std::vector<VkCommandBuffer> CommandPool::allocateBuffers(uint32_t count, VkCommandBufferLevel level)
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
    }

    VkCommandBuffer CommandPool::allocateSingleBuffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY)
    {
        return allocateBuffers(1, level)[0];
    }

    void CommandPool::freeBuffers(const std::vector<VkCommandBuffer>& buffers)
    {
        vkFreeCommandBuffers(m_deviceRef.handle(), m_commandPool, buffers.size(), buffers.data());
    }

    VkCommandBuffer CommandPool::beginSingleTimeCommand() const
    {
        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = m_commandPool;
        allocateInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(m_deviceRef.handle(), &allocateInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void CommandPool::endSingleTimeCommand(VkCommandBuffer commandBuffer, VkQueue queue) const
    {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(queue);

        vkFreeCommandBuffers(m_deviceRef.handle(), m_commandPool, 1, &commandBuffer);
    }
}