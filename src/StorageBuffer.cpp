#include "StorageBuffer.hpp"

#include <iostream>
#include <cstring>

StorageBuffer::StorageBuffer(const void* data, unsigned int size, VkDevice device, VkPhysicalDevice physicalDevice, VkDescriptorPool descriptorPool, VkQueue graphicsQueue, VkCommandPool commandPool) : Buffer(device, physicalDevice, descriptorPool) {
    this->device = device;
    this->physicalDevice = physicalDevice;
    this->descriptorPool = descriptorPool;
    this->graphicsQueue = graphicsQueue;
    this->commandPool = commandPool;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    // Staging buffer.
    createBuffer(size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    // Copy data to mapped memory.
    void* mappedMemory;
    vkMapMemory(device, stagingBufferMemory, 0, size, 0, &mappedMemory);
    memcpy(mappedMemory, data, size);
    vkUnmapMemory(device, stagingBufferMemory);

    // Actual buffer.
    createBuffer(size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &buffer, &bufferMemory);
    
    // Copy from staging buffer to actual buffer.
    copyBuffer(stagingBuffer, buffer, size);

    // Create descriptor set.
    createDescriptorSetLayout(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT);
    createDescriptorSet(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, buffer, size);
    
    // Destroy staging buffer.
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

StorageBuffer::~StorageBuffer() {
    vkDestroyBuffer(device, buffer, nullptr);
    vkFreeMemory(device, bufferMemory, nullptr);
}

void StorageBuffer::copyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size){
    // Create command buffer allocation info.
    VkCommandBufferAllocateInfo allocationInfo = {};
    allocationInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocationInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocationInfo.commandPool = commandPool;
    allocationInfo.commandBufferCount = 1;

    // Create command buffer.
    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocationInfo, &commandBuffer);

    // Command buffer begin info.
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    // Start recording commands.
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion = {};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, source, destination, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}
