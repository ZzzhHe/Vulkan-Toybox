#include "texture_lib.h"

#include "core/device.h"
#include "resources/memory/memory_allocator.h"
#include "resources/images/texture.h"
#include "graphics/command_pool.h"

namespace vkcommon {

    TextureLibrary::TextureLibrary(const Device& device, MemoryAllocator& allocator)
        : m_deviceRef(device), m_allocatorRef(allocator) {
    }

    std::shared_ptr<Texture> TextureLibrary::getOrLoadTexture(const std::filesystem::path& path, const CommandPool& cmdPool)
    {
        auto it = m_texturesMap.find(path);
        if (it != m_texturesMap.end()) {
            return it->second;
        }

        auto texture = std::make_shared<Texture>(m_deviceRef, m_allocatorRef);
        texture->loadFromFile(path, cmdPool);
        texture->createSampler();

        m_texturesMap[path] = texture;
        return texture;
    }

} // namespace vkcommon