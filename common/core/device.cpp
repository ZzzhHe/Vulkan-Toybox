#include "device.h"

#include "core/physical_device.h"
#include "core/instance.h"

#include <set>
#include <vector>
#include <stdexcept>

namespace vkcommon
{

    Device::Device(const PhysicalDevice& physicalDevice)
        : m_physicalDeviceRef(physicalDevice)
    {
        QueueFamilyIndices indices = m_physicalDeviceRef.queueFamilyIndices();
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        // Set: If both the graphics and presentation families are the same, only need one queue for them.
        std::set<uint32_t> uniqueQueueFamilies = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value() };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.sampleRateShading = VK_TRUE;
        deviceFeatures.geometryShader = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(kDeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = kDeviceExtensions.data();

        // validation layer setting can be ignored in new implementation and
        // the settings here are the same as the instance
#ifdef NODEBUG
        createInfo.enabledLayerCount = 0;
#else
        createInfo.enabledLayerCount = static_cast<uint32_t>(kValidationLayers.size());
        createInfo.ppEnabledLayerNames = kValidationLayers.data();
#endif

        if (vkCreateDevice(m_physicalDeviceRef.handle(), &createInfo, nullptr, &m_device) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create logical device!");
        }
        // retrieve queue
        // create queue during creating logical device, but didn't return it, so we need to retrieve it
        // in order to use it later
        vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
        vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
    }

    Device::~Device()
    {
        if (m_device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(m_device, nullptr);
        }
    }

    VkFormatProperties Device::physicalDeviceFormatProperties(VkFormat format) const
    {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(m_physicalDeviceRef.handle(), format, &formatProperties);
        return formatProperties;
    }

    VkFormat Device::findDepthFormat() const
    {
        return m_physicalDeviceRef.findDepthFormat();
    }

    VkSampleCountFlagBits Device::msaaSamples() const
    {
        return m_physicalDeviceRef.msaaSamples(); 
    }

} // namespace vkcommon