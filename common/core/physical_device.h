#ifndef PHYSICAL_DEVICE_H
#define PHYSICAL_DEVICE_H

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

namespace vkcommon
{
    class Instance;
    class Surface;

    const std::vector<const char*> kDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  #ifdef __APPLE__
        "VK_KHR_portability_subset"
  #endif // __APPLE__
    };

    struct QueueFamilyIndices
    {
        // std::optional is a wrapper that contains a value or nothing
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    class PhysicalDevice
    {
    public:
        PhysicalDevice(const Instance& instance, const Surface& surface);
        ~PhysicalDevice();

        PhysicalDevice(const PhysicalDevice&) = delete;
        PhysicalDevice& operator=(const PhysicalDevice&) = delete;

        VkPhysicalDevice handle() const { return m_physicalDevice; }
        VkSampleCountFlagBits msaaSamples() const { return m_msaaSamples; }
        QueueFamilyIndices queueFamilyIndices() const { return m_indices; }

        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
        VkFormat findDepthFormat() const;
        bool hasStencilComponent(VkFormat format) const;

    private:
        bool checkDeviceExtensionSupport();
        bool isDeviceSuitable();
        QueueFamilyIndices findQueueFamilies();
        VkSampleCountFlagBits getMaxUsableSampleCount();

    private:
        const Instance& m_instanceRef;
        const Surface& m_surfaceRef;

        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        QueueFamilyIndices m_indices;
        VkSampleCountFlagBits m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    };
} // namespace vkcommon

#endif // PHYSICAL_DEVICE_H