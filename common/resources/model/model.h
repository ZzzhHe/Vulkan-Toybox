#ifndef MODEL_H
#define MODEL_H

#include <memory>
#include <vector>
#include <filesystem>
#include <vulkan/vulkan.h>

class aiNode;
struct aiScene;
class aiMesh;
class aiMaterial;

namespace vkcommon {
    class Device;
    class MemoryAllocator;
    class CommandPool;
    class Mesh;
    class Material;
    class TextureLibrary;
    class DescriptorSetLayout;
    class DescriptorPool;
    class DescriptorWriter;

    class Model {
    public:
        Model(const Device& device, MemoryAllocator& allocator);
        ~Model() = default;

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        Model(Model&& other) noexcept;
        Model& operator=(Model&& other) noexcept;

        void loadFromFile(
            const std::filesystem::path& path,
            TextureLibrary& textureLib,
            const CommandPool& cmdPool
        );

        void createDescriptor(
            DescriptorPool& pool,
            const DescriptorSetLayout& materialLayout,
            uint32_t framesInFlight);

        void updateProperties(uint32_t currentFrame);
        
        void draw(
            VkCommandBuffer commandBuffer,
            uint32_t currentFrame,
            VkPipelineLayout pipelineLayout);

        // Getters for model properties
        const std::vector<std::shared_ptr<Mesh>>& getMeshes() const { return m_meshes; }
        bool isLoaded() const { return !m_meshes.empty(); }

    private:
        const Device& m_deviceRef;
        MemoryAllocator& m_allocatorRef;
        std::vector<std::shared_ptr<Mesh>> m_meshes;

        void loadNode(
            const aiNode* node,
            const aiScene* scene,
            TextureLibrary& textureLib,
            const CommandPool& cmdPool,
            const std::filesystem::path& modelPath
        );

        void processMesh(
            const aiMesh* mesh,
            const aiScene* scene,
            TextureLibrary& textureLib,
            const CommandPool& cmdPool,
            const std::filesystem::path& modelPath
        );

        void loadMaterialTextures(
            const aiMaterial* material,
            TextureLibrary& textureLib,
            const CommandPool& cmdPool,
            const std::filesystem::path& modelPath,
            std::shared_ptr<Material> vkMaterial
        );
    };

} // namespace vkcommon

#endif // MODEL_H