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

        ShaderModule(const ShaderModule&) = delete;
        ShaderModule& operator=(const ShaderModule&) = delete;

        operator VkShaderModule() const noexcept { return m_module; }

    private:
        std::vector<char> readFile(const std::filesystem::path& filename) const;

        VkShaderModule m_module;
        const Device& m_deviceRef;
    };
}

#endif