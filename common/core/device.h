#ifndef DEVICE_H
#define DEVICE_H

#include <vulkan/vulkan.h>

namespace vkcommon
{
    class PhysicalDevice;

    class Device
    {
    public:
        Device(const PhysicalDevice& physicalDevice);
        ~Device();

        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;

        VkDevice handle() const { return m_device; }
        VkQueue graphicsQueue() const { return m_graphicsQueue; }
        VkQueue presentQueue() const { return m_presentQueue; }

        VkFormatProperties physicalDeviceFormatProperties(VkFormat format) const;
        VkFormat findDepthFormat() const;
        VkSampleCountFlagBits msaaSamples() const;
        const PhysicalDevice& physicalDevice() const { return m_physicalDeviceRef; }

    private:
        VkDevice m_device = VK_NULL_HANDLE;
        VkQueue m_graphicsQueue;
        VkQueue m_presentQueue;

        const PhysicalDevice& m_physicalDeviceRef;
    };

} // namespace vkcommon

#endif // DEVICE_H