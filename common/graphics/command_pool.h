#ifndef COMMAND_POOL_H
#define COMMAND_POOL_H

#include <vulkan/vulkan.h>
#include <vector>

namespace vkcommon
{
    class PhysicalDevice;
    class Device;

    class CommandPool
    {
    public:
        CommandPool(const PhysicalDevice& PhysicalDevice, const Device& device);
        ~CommandPool();

        CommandPool(const CommandPool&) = delete;
        CommandPool& operator=(const CommandPool&) = delete;

        // Command buffer management
        std::vector<VkCommandBuffer> allocateBuffers(uint32_t count, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        VkCommandBuffer allocateSingleBuffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        void freeBuffers(const std::vector<VkCommandBuffer>& buffers);

        // Immediate command execution
        VkCommandBuffer beginSingleTimeCommand() const;
        void endSingleTimeCommand(VkCommandBuffer commandBuffer, VkQueue queue) const;

        VkCommandPool handle() const noexcept { return m_commandPool; }

    private:
        VkCommandPool m_commandPool;

        const Device& m_deviceRef;
    };
}

#endif // COMMAND_POOL_H
