#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

namespace vkcommon {

    class UniformBuffer;
    class Texture;
    class Device;
    class MemoryAllocator;
    class DescriptorPool;
    class DescriptorSetLayout;


    struct MaterialProperties {
        alignas(16) glm::vec4 ambientColor;
        alignas(16) glm::vec4 diffuseColor;
        alignas(16) glm::vec4 specularColor;
        alignas(16) glm::vec4 emissiveColor;
        alignas(4) float shininess;
        alignas(4) float opacity;
        alignas(4) float refractiveIndex;
    };

    class Material {
    public:
        Material(const Device& device, MemoryAllocator& allocator);
        ~Material() = default;

        Material(const Material&) = delete;
        Material& operator=(const Material&) = delete;
        Material(Material&& other) = default;
        Material& operator=(Material&& other) = default;

        // Descriptor management
        static void createDescriptorLayout(const Device& device);
        static void destroyDescriptorLayout();
        static std::unique_ptr<DescriptorSetLayout>& getDescriptorLayout() { return s_descriptorSetLayout; }
        
        void initDescriptorSets(DescriptorPool& pool, const DescriptorSetLayout& layout, uint32_t framesInFlight);

        void createPropertiesUBO(const int MAX_FRAMES_IN_FLIGHT);

        void updateProperties(uint32_t currentImage);
        //void updateTextures(uint32_t currentFrame);

        friend class Model;
        friend class Mesh;  

    private:
        static std::unique_ptr<DescriptorSetLayout> s_descriptorSetLayout;
        std::vector<VkDescriptorSet> m_descriptorSets;

        MaterialProperties m_properties;
        std::unique_ptr<UniformBuffer> m_ubo;

        std::shared_ptr<Texture> m_diffuseMap{ nullptr };
        std::shared_ptr<Texture> m_specularMap{ nullptr };
        std::shared_ptr<Texture> m_normalMap{ nullptr };

        const Device& m_deviceRef;
    };  

} // namespace vkcommon

#endif // MATERIAL_H