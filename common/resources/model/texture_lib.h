#ifndef TEXTURE_LIB_H
#define TEXTURE_LIB_H

#include <memory>
#include <unordered_map>
#include <filesystem>

namespace vkcommon
{
    class Device;
    class MemoryAllocator;
    class Texture;
    class CommandPool;

    class TextureLibrary {
    public:
        TextureLibrary(const Device& device, MemoryAllocator& allocator);
        ~TextureLibrary() = default;

        TextureLibrary(const TextureLibrary&) = delete;
        TextureLibrary& operator=(const TextureLibrary&) = delete;

        TextureLibrary(TextureLibrary&& other) = delete;
        TextureLibrary& operator=(TextureLibrary&& other) = delete;

        std::shared_ptr<Texture> getOrLoadTexture(
            const std::filesystem::path& path,
            const CommandPool& cmdPool
        );

    private:
        const Device& m_deviceRef;
        MemoryAllocator& m_allocatorRef;

        std::unordered_map<std::filesystem::path, std::shared_ptr<Texture>> m_texturesMap;
    };

} // namespace vkcommon

#endif // TEXTURE_LIB_H