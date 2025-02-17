#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vector>
#include <vulkan/vulkan.h>

#include "core/window.h"

namespace vkcommon
{
    class Window;
    class Surface;
    class PhysicalDevice;
    class Device;

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class SwapChain
    {
    public:
        SwapChain(const Window& window, const Surface& surface,
            const PhysicalDevice& physicalDevice, const Device& device);
        ~SwapChain();

        void createSwapChain();
        void createImageViews();
        void destroySwapChain() const;

        void createFrameBuffers(const VkRenderPass& renderPass, VkImageView colorImageView, VkImageView depthImageView);
        void destroyFrameBuffers();
        
        VkSwapchainKHR handle() const { return m_swapChain; }
        std::vector<VkImage> swapChainImages() const { return m_swapChainImages; }
        VkImageView swapChainImageView(int index) const { return m_swapChainImageViews[index]; }
        std::vector<VkImageView> swapChainImageViews() const { return m_swapChainImageViews; }
        VkFormat swapChainImageFormat() const { return m_swapChainImageFormat; }
        VkExtent2D swapChainExtent() const { return m_swapChainExtent; }
        VkFramebuffer swapChainFramebuffer(size_t index) const { return m_swapChainFramebuffers[index]; }

    private:
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);

        VkSwapchainKHR m_swapChain;
        std::vector<VkImage> m_swapChainImages;
        VkFormat m_swapChainImageFormat;
        VkExtent2D m_swapChainExtent;
        std::vector<VkImageView> m_swapChainImageViews;
        std::vector<VkFramebuffer> m_swapChainFramebuffers;

        const Window& m_windowRef;
        const Surface& m_surfaceRef;
        const PhysicalDevice& m_physicalDeviceRef;
        const Device& m_deviceRef;
    };

} // namespace vkcommon

#endif // SWAPCHAIN_H
