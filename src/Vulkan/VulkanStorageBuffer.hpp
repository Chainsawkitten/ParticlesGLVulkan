#pragma once

#include <vulkan/vulkan.h>

/// Used for storing particle data.
class VulkanStorageBuffer {
    public:
        /// Create new storage buffer.
        /**
         * @param data Buffer data.
         * @param size Buffer size.
         * @param device Logical Vulkan device.
         * @param physicalDevice Physical Vulkan device.
         * @param descriptorPool Pool to allocate descriptor sets from.
         */
        VulkanStorageBuffer(const void* data, unsigned int size, VkDevice device, VkPhysicalDevice physicalDevice, VkDescriptorPool descriptorPool);
        
        /// Destructor.
        ~VulkanStorageBuffer();
        
    private:
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
        void createDescriptorSetLayout();
        void createDescriptorSet(VkDeviceSize size);
        
        VkDevice device;
        VkPhysicalDevice physicalDevice;
        VkDescriptorPool descriptorPool;
        
        VkBuffer buffer;
        VkDeviceMemory bufferMemory;
        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorSet descriptorSet;
};