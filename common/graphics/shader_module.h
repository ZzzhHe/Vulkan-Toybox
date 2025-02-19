#ifndef SHADER_MODULE_H
#define SHADER_MODULE_H

#include <vulkan/vulkan.h>

#include <filesystem>
#include <vector>

namespace vkcommon
{
    class Device;

    class ShaderModule
    {
    public:
        ShaderModule(const Device& device, const std::filesystem::path& spirvPath);
        ~ShaderModule();
        
        // Disable copy semantics
        ShaderModule(const ShaderModule&) = delete;
        ShaderModule& operator=(const ShaderModule&) = delete;

        // Enable move semantics
        ShaderModule(ShaderModule&& other) noexcept;
        ShaderModule& operator=(ShaderModule&& other) noexcept;

        operator VkShaderModule() const noexcept { return m_module; }

    private:
        void destroy() noexcept;
        std::vector<char> readFile(const std::filesystem::path& filename) const;

        VkShaderModule m_module;
        const Device& m_deviceRef;
    };
}

#endif