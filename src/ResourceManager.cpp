/*
* Vulkan Invaders - Resource manager class
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "ResourceManager.h"

ResourceManager::ResourceManager(VulkanDevice &device, VkQueue queue, VkCommandPool commandPool) : device(device), queue(queue), commandPool(commandPool) { }

ResourceManager::~ResourceManager()
{
	for (auto model : models) {
		delete model;
	}
}

Model *ResourceManager::loadModel(std::string id, std::string filename)
{
	Model *model = new Model(id, filename, device, queue, commandPool);
	models.push_back(model);
	return model;
}
