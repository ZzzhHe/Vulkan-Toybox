#ifndef INSTANCE_H
#define INSTANCE_H

#include <vulkan/vulkan.h>
#include <vector>

namespace vkcommon
{
    const std::vector<const char*> kValidationLayers = {
        "VK_LAYER_KHRONOS_validation" };

    class Instance
    {
    public:
        Instance();
        ~Instance();

        VkInstance handle() const { return m_instance; }
        bool hasValidationLayers() const { return m_hasValidationLayers; }

        // list of physical devices
        std::vector<VkPhysicalDevice> enumeratePhysicalDevices() const;

    private:
        // Instance creation helpers
        void createInstance();
        void setupDebugMessenger();

        bool checkValidationLayerSupport();
        std::vector<const char*> getRequiredExtensions();

        // Debug messenger helpers
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            [[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            [[maybe_unused]] void* pUserData);

        static VkResult createDebugUtilsMessengerEXT(
            VkInstance instance,
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkDebugUtilsMessengerEXT* pDebugMessenger);

        static void destroyDebugUtilsMessengerEXT(
            VkInstance instance,
            VkDebugUtilsMessengerEXT debugMessenger,
            const VkAllocationCallbacks* pAllocator);

    private:
        VkInstance m_instance{ VK_NULL_HANDLE };
        VkDebugUtilsMessengerEXT m_debugMessenger{ VK_NULL_HANDLE };
        bool m_hasValidationLayers{ false };

        // Configuration
        static constexpr const char* const kAppName = "Vulkan Application";
        static constexpr const char* const kEngineName = "No Engine";
    };

} // namespace vkcommon

#endif // INSTANCE_H