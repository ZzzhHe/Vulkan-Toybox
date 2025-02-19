#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include <array>
#include <vector>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vulkan/vulkan_core.h>

#include "resources/buffers/buffer.h"

namespace vkcommon
{
    class Device;
    class CommandPool;
    class MemoryAllocator;
    class Buffer;

    struct Vertex {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 texCoord;

        static VkVertexInputBindingDescription getBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
    };

    class VertexBuffer
    {
    public:
        VertexBuffer(const Device& device, MemoryAllocator& allocator);
        ~VertexBuffer();

        // Disable copying
        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;

        // Enable moving
        VertexBuffer(VertexBuffer&& other) noexcept;
        VertexBuffer& operator=(VertexBuffer&& other) noexcept;

        void createVertexBuffer(const std::vector<Vertex>& vertices, const CommandPool& cmdPool);
        void createIndexBuffer(const std::vector<uint32_t>& indices, const CommandPool& cmdPool);

        void bindVertexBuffer(VkCommandBuffer commandBuffer, uint32_t firstBinding);
        void bindIndexBuffer(VkCommandBuffer commandBuffer, VkIndexType indexType);

        VkBuffer vertexBuffer() const { return m_vertexBuffer.handle(); }
        VkBuffer indexBuffer() const { return m_indexBuffer.handle(); }

    private:
        const Device& m_device;
        MemoryAllocator& m_allocator;

        Buffer m_vertexBuffer;
        Buffer m_indexBuffer;
    };
} // namespace vkcommon

#endif // VERTEXBUFFER_H
