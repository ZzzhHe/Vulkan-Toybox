#ifndef TEXTURE_H
#define TEXTURE_H

#include <vulkan/vulkan_core.h>

#include <memory>

#include "resources/images/image.h"
#include "resources/buffers/buffer.h"

namespace vkcommon {

    class PhysicalDevice;
    class Device;
    class CommandPool;
    class MemoryAllocator;

    class Texture {
    public:
        Texture(const Device& device, MemoryAllocator& allocator);
        ~Texture();

        // Disable copying
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        // Enable moving
        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        // Load texture from file
        void loadFromFile(const std::string& filepath, const CommandPool& commandPool);

        void createSampler(float maxAnisotropy = 16.0f,
            VkFilter minFilter = VK_FILTER_LINEAR,
            VkFilter magFilter = VK_FILTER_LINEAR,
            VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);

        VkImage imageHandle() const { return m_image.handle(); }
        VkImageView imageView() const { return m_imageView; }
        VkSampler sampler() const { return m_sampler; }
        uint32_t mipLevels() const { return m_image.mipLevels(); }
        VkExtent2D extent() const { return m_image.extent(); }
        VkFormat format() const { return m_image.format(); }

    private:
        void generateMipMaps(const CommandPool& cmdPool);
        void cleanup();

        std::unique_ptr<Buffer> m_stagingBuffer;

        Image m_image;
        VkImageView m_imageView{ VK_NULL_HANDLE };
        VkSampler m_sampler{ VK_NULL_HANDLE };

        const Device& m_deviceRef;
        MemoryAllocator& m_allocatorRef;
    };

} // namespace vkcommon

#endif //TEXTURE_H
