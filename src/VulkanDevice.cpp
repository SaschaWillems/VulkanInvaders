/*
* Vulkan Invaders - Vulkan device wrapper class
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include "VulkanDevice.h"

VulkanDevice::VulkanDevice(VkPhysicalDevice physicalDevice, VkDevice device)
{
	this->physicalDevice = physicalDevice;
	this->device = device;
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);
	vkGetPhysicalDeviceFeatures(physicalDevice, &features);
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
}

VulkanDevice::~VulkanDevice()
{
}

uint32_t VulkanDevice::getMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties)
{
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		if ((typeBits & 1) == 1) {
			if ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		typeBits >>= 1;
	}
	throw std::runtime_error("Could not find a matching memory type");
}

VulkanDeviceBuffer VulkanDevice::createBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, void *data)
{
	VulkanDeviceBuffer buffer;

	VkBufferCreateInfo bufferCI{};
	bufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCI.size = size;
	bufferCI.usage = usageFlags;
	VK_CHECK_RESULT(vkCreateBuffer(device, &bufferCI, nullptr, &buffer.buffer));

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(device, buffer.buffer, &memoryRequirements);
	VkMemoryAllocateInfo memoryAllocateInfo{};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = getMemoryTypeIndex(memoryRequirements.memoryTypeBits, memoryPropertyFlags);
	VK_CHECK_RESULT(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &buffer.memory));
	buffer.size = memoryAllocateInfo.allocationSize;

	if (memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
		VK_CHECK_RESULT(vkMapMemory(device, buffer.memory, 0, size, 0, &buffer.mapped));
	}
	if (data != nullptr) {
		memcpy(buffer.mapped, data, size);
		if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) {
			VkMappedMemoryRange mappedMemoryRange{};
			mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;;
			mappedMemoryRange.memory = buffer.memory;
			mappedMemoryRange.size = size;
			vkFlushMappedMemoryRanges(device, 1, &mappedMemoryRange);
		}
	}

	VK_CHECK_RESULT(vkBindBufferMemory(device, buffer.buffer, buffer.memory, 0));

	return buffer;
}