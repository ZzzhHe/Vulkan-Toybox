#include "shader_module.h"

#include "core/device.h"

#include <fstream>
#include <stdexcept>

namespace vkcommon
{
    ShaderModule::ShaderModule(const Device& device, const std::filesystem::path& spirvPath)
        : m_deviceRef(device)
    {
        auto code = readFile(spirvPath);

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(m_deviceRef.handle(), &createInfo, nullptr, &m_module) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shader module!");
        }
    }

    ShaderModule::~ShaderModule()
    {
        destroy();
    }

    void ShaderModule::destroy() noexcept
    {
        if (m_module != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(m_deviceRef.handle(), m_module, nullptr);
            m_module = VK_NULL_HANDLE;
        }
    }

    ShaderModule::ShaderModule(ShaderModule&& other) noexcept
        : m_deviceRef(other.m_deviceRef),
        m_module(other.m_module)
    {   
        other.m_module = VK_NULL_HANDLE;
    }

    ShaderModule& ShaderModule::operator=(ShaderModule&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            m_module = other.m_module;
            other.m_module = VK_NULL_HANDLE;
        }
        return *this;
    }

    std::vector<char> ShaderModule::readFile(const std::filesystem::path& path) const
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file: " + path.string());
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }
} // namespace vkcommon