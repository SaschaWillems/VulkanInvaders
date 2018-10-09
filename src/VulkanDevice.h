/*
* Vulkan Invaders - Vulkan device wrapper class
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include <string>
#include <iostream>
#include <assert.h>
#include "vulkan/vulkan.h"

#include "VulkanUtils.h"

class VulkanDeviceBuffer {
public:
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDeviceSize size;
	void* mapped = nullptr;
};

class VulkanDevice
{
private:
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	VkPhysicalDeviceMemoryProperties memoryProperties;
public:
	operator VkDevice() { return device; };
	operator VkPhysicalDevice() { return physicalDevice; };

	VulkanDevice(VkPhysicalDevice physicalDevice, VkDevice device);
	~VulkanDevice();

	uint32_t getMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties);
	VulkanDeviceBuffer createBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, void *data = nullptr);
};

