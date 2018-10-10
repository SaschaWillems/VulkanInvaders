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
		delete model.second;
	}
	for (auto shader : shaders) {
		delete shader.second;
	}
}

Model *ResourceManager::loadModel(std::string id, std::string filename)
{
	Model *model = new Model(id, filename, device, queue, commandPool);
	models.emplace(id, model);
	return model;
}

Shader *ResourceManager::loadShader(std::string id, std::string filename)
{
	Shader* shader = new Shader(id, filename, device);
	shaders.emplace(id, shader);
	return shader;
}

Shader *ResourceManager::getShader(std::string id)
{
	auto it = shaders.find(id);
	assert(it != shaders.end());
	return it->second;
}

Model *ResourceManager::getModel(std::string id)
{
	auto it = models.find(id);
	assert(it != models.end());
	return it->second;
}
