/*
* Vulkan Invaders - Resource manager class
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include <string>
#include <vector>
#include "vulkan/vulkan.h"

#include "VulkanDevice.h"
#include "Model.h"

class ResourceManager
{
private:
	VulkanDevice &device;
	VkCommandPool commandPool;
	VkQueue queue;
public:
	ResourceManager(VulkanDevice &device, VkQueue queue, VkCommandPool commandPool);
	~ResourceManager();

	std::vector<Model*> models;

	Model* loadModel(std::string id, std::string filename);
};
