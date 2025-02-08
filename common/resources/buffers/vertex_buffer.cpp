#include "vertex_buffer.h"

#include "core/device.h"
#include "graphics/command_pool.h"
#include "resources/buffers/buffer.h"
#include "resources/memory/memory_allocator.h"

namespace vkcommon {
    VkVertexInputBindingDescription Vertex::getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    std::array<VkVertexInputAttributeDescription, 3> Vertex::getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }

    VertexBuffer::VertexBuffer(const Device& device, MemoryAllocator& allocator)
        : m_device(device)
        , m_allocator(allocator)
        , m_vertexBuffer(device, allocator)
        , m_indexBuffer(device, allocator)
        , m_stagingBuffer(device, allocator) {
    }

    VertexBuffer::~VertexBuffer() = default;

    VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
        : m_device(other.m_device)
        , m_allocator(other.m_allocator)
        , m_vertexBuffer(std::move(other.m_vertexBuffer))
        , m_indexBuffer(std::move(other.m_indexBuffer))
        , m_stagingBuffer(std::move(other.m_stagingBuffer)) {
    }

    VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept {
        if (this != &other) {
            m_vertexBuffer = std::move(other.m_vertexBuffer);
            m_indexBuffer = std::move(other.m_indexBuffer);
            m_stagingBuffer = std::move(other.m_stagingBuffer);
        }
        return *this;
    }

    void VertexBuffer::createVertexBuffer(const std::vector<Vertex>& vertices,
        const CommandPool& cmdPool) {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        // Create staging buffer
        m_stagingBuffer.create(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        // Copy vertex data to staging buffer
        m_stagingBuffer.update(vertices.data(), bufferSize);

        // Create vertex buffer
        m_vertexBuffer.create(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        // Copy from staging to vertex buffer
        m_vertexBuffer.copyFrom(m_stagingBuffer, bufferSize, cmdPool);
    }

    void VertexBuffer::createIndexBuffer(const std::vector<uint32_t>& indices,
        const CommandPool& cmdPool) {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        // Create staging buffer
        m_stagingBuffer.create(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        // Copy index data to staging buffer
        m_stagingBuffer.update(indices.data(), bufferSize);

        // Create index buffer
        m_indexBuffer.create(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        // Copy from staging to index buffer
        m_indexBuffer.copyFrom(m_stagingBuffer, bufferSize, cmdPool);
    }
} // namespace vkcommon