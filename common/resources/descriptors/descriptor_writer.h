#ifndef DESCRIPTOR_SET_LAYOUT_H
#define DESCRIPTOR_SET_LAYOUT_H

#include <vulkan/vulkan.h>
#include <vector>

namespace vkcommon {
    class DescriptorWriter {
    public:
        DescriptorWriter(VkDescriptorSet descriptorSet);

        DescriptorWriter& writeBuffer(uint32_t binding,
            VkDescriptorType type,
            VkBuffer buffer,
            VkDeviceSize size,
            VkDeviceSize offset = 0);

        DescriptorWriter& writeImage(uint32_t binding,
            VkDescriptorType type,
            VkImageView imageView,
            VkSampler sampler,
            VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        void update(const Device& device);

    private:
        VkDescriptorSet m_descriptorSet;
        std::vector<VkWriteDescriptorSet> m_writes;
        std::vector<VkDescriptorBufferInfo> m_bufferInfos;
        std::vector<VkDescriptorImageInfo> m_imageInfos;
    };

} // namespace vkcommon

#endif // DESCRIPTOR_SET_LAYOUT_H