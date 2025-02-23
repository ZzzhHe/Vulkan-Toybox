#include "descriptor_writer.h"

#include "core/device.h"

namespace vkcommon {
    DescriptorWriter::DescriptorWriter(VkDescriptorSet descriptorSet)
        : m_descriptorSet(descriptorSet) {
    }

    DescriptorWriter& DescriptorWriter::writeBuffer(
        uint32_t binding,
        VkDescriptorType type,
        VkBuffer buffer,
        VkDeviceSize size,
        VkDeviceSize offset) {

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer;
        bufferInfo.offset = offset;
        bufferInfo.range = size;
        m_bufferInfos.push_back(bufferInfo);

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = m_descriptorSet;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.descriptorType = type;
        write.descriptorCount = 1;
        write.pBufferInfo = nullptr; // Will be set later in update() function
        m_writes.push_back(write);

        return *this;
    }

    DescriptorWriter& DescriptorWriter::writeImage(
        uint32_t binding,
        VkDescriptorType type,
        VkImageView imageView,
        VkSampler sampler,
        VkImageLayout imageLayout) {

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = imageLayout;
        imageInfo.imageView = imageView;
        imageInfo.sampler = sampler;
        m_imageInfos.push_back(imageInfo);

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = m_descriptorSet;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.descriptorType = type;
        write.descriptorCount = 1;
        write.pImageInfo = nullptr; // Will be set later in update() function
        m_writes.push_back(write);

        return *this;
    }

    void DescriptorWriter::update(const Device& device) {
        size_t bufferIndex = 0;
        size_t imageIndex = 0;
        for (auto& write : m_writes) {
            if (write.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
                write.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
                write.pBufferInfo = &m_bufferInfos[bufferIndex];
            }
            else if (write.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
                write.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE) {
                write.pImageInfo = &m_imageInfos[imageIndex];
            }
        }

        vkUpdateDescriptorSets(
            device.handle(),
            static_cast<uint32_t>(m_writes.size()),
            m_writes.data(),
            0,
            nullptr
        );
    }
} // namespace vkcommon