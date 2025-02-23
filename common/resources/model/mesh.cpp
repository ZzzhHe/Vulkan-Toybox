#include "mesh.h"

#include "resources/model/material.h"
#include "resources/buffers/uniform_buffer.h"
#include "resources/buffers/vertex_buffer.h"
#include "resources/model/material.h"

namespace vkcommon {

    Mesh::Mesh(const Device& device, MemoryAllocator& allocator) {
        m_vertexBuffer = std::make_unique<VertexBuffer>(device, allocator);
        m_material = std::make_shared<Material>(device, allocator);
    }

    Mesh::Mesh(Mesh&& other) noexcept :
        m_vertexBuffer(std::move(other.m_vertexBuffer)),
        m_indexCount(other.m_indexCount),
        m_material(std::move(other.m_material)) {
    }

    Mesh& Mesh::operator=(Mesh&& other) noexcept {
        if (this != &other) {
            m_vertexBuffer = std::move(other.m_vertexBuffer);
            m_indexCount = other.m_indexCount;
            m_material = std::move(other.m_material);
        }
        return *this;
    }

    void Mesh::createVertexBuffer(
        const std::vector<Vertex>& vertices, 
        const std::vector<uint32_t>& indices, 
        const CommandPool& cmdPool) {
        m_vertexBuffer->createVertexBuffer(vertices, cmdPool);
        m_vertexBuffer->createIndexBuffer(indices, cmdPool);
        m_indexCount = static_cast<uint32_t>(indices.size());
    }

    void Mesh::draw(
        VkCommandBuffer commandBuffer, 
        uint32_t currentFrame,
        VkPipelineLayout pipelineLayout) {
        m_vertexBuffer->bindVertexBuffer(commandBuffer, 0);
        m_vertexBuffer->bindIndexBuffer(commandBuffer, VK_INDEX_TYPE_UINT32);
        
        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            1,  // Material Descriptors : 2
            1,  // One set
            &m_material->m_descriptorSets[currentFrame],
            0,
            nullptr
        );

        vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
    }

} // namespace vkcommon