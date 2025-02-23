#ifndef MESH_H
#define MESH_H

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

namespace vkcommon {

    struct Vertex;
    class VertexBuffer;
    class Material;
    class Device;
    class MemoryAllocator;
    class CommandPool;

    class Mesh {
    public:
        Mesh(const Device& device, MemoryAllocator& allocator);
        ~Mesh() = default;

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;

        void createVertexBuffer(
            const std::vector<Vertex>& vertices, 
            const std::vector<uint32_t>& indices, 
            const CommandPool& cmdPool);

        void draw(
            VkCommandBuffer commandBuffer,
            uint32_t currentFrame, 
            VkPipelineLayout pipelineLayout);
    
        friend class Model;

    private:
        std::unique_ptr<VertexBuffer> m_vertexBuffer;
        std::shared_ptr<Material> m_material;
        uint32_t m_indexCount{ 0 };
    };

} // namespace vkcommon

#endif // MESH_H