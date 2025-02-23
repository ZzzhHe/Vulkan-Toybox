#include "material.h"

#include "core/device.h"
#include "resources/memory/memory_allocator.h"
#include "resources/images/texture.h"
#include "resources/buffers/uniform_buffer.h"
#include "resources/descriptors/descriptor_set_layout.h"
#include "resources/descriptors/descriptor_pool.h"
#include "resources/descriptors/descriptor_writer.h"

#include <stdexcept>

namespace vkcommon {
    std::unique_ptr<DescriptorSetLayout> Material::s_descriptorSetLayout;

    Material::Material(const Device& device, MemoryAllocator& allocator)
        : m_deviceRef(device) {
        m_properties = {};
        m_ubo = std::make_unique<UniformBuffer>(m_deviceRef, allocator);
    }

    void Material::createDescriptorSetLayout(const Device& device) {

        auto layout = std::make_unique<DescriptorSetLayout>(device);
        
        // Material's properties 
        layout->addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
        // Material's textures
        layout->addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            VK_SHADER_STAGE_FRAGMENT_BIT); // Diffuse
        layout->addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            VK_SHADER_STAGE_FRAGMENT_BIT); // Specular
        layout->addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            VK_SHADER_STAGE_FRAGMENT_BIT); // Normal

        layout->create();
        
        // set the static layout by moving the created layout
        s_descriptorSetLayout = std::move(layout);
    }

    void Material::destroyDescriptorSetLayout()
    {
        s_descriptorSetLayout.reset();
    }

    void Material::createDescriptorSets(DescriptorPool& pool, const DescriptorSetLayout& layout, uint32_t framesInFlight)
    {
        m_descriptorSets = pool.allocate(layout.handle(), framesInFlight);

        for (uint32_t frameIndex = 0; frameIndex < framesInFlight; frameIndex++) {
            DescriptorWriter writer{ m_descriptorSets[frameIndex] };

            writer.writeBuffer(
                0,
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                m_ubo->buffer(frameIndex),
                sizeof(MaterialProperties));

            if (m_diffuseMap != nullptr) {
                writer.writeImage(
                    1,
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    m_diffuseMap->imageView(),
                    m_diffuseMap->sampler());
            }

            if (m_specularMap != nullptr) {
                writer.writeImage(
                    2,
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    m_specularMap->imageView(),
                    m_specularMap->sampler());
            }

            if (m_normalMap != nullptr) {
                writer.writeImage(
                    3,
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    m_normalMap->imageView(),
                    m_normalMap->sampler());
            }

            writer.update(m_deviceRef);
        }

    }

    void Material::createPropertiesUBO(const int MAX_FRAMES_IN_FLIGHT) {
        m_ubo->create(sizeof(MaterialProperties), MAX_FRAMES_IN_FLIGHT);
    }

    void Material::updateProperties(uint32_t currentFrame) {
        if (m_ubo == nullptr) {
            throw std::runtime_error("Material UBO is not created!");
        }
        m_ubo->updateData(currentFrame, &m_properties, sizeof(MaterialProperties));
    }

    //void Material::updateTextures(uint32_t currentFrame)
    //{
    //    // only update when textures change from each frame.
    //}

}; // namespace vkcommon