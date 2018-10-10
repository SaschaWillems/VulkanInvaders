/*
* Vulkan Invaders - Resource manager class
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include <string>
#include <unordered_map>
#include "vulkan/vulkan.h"

#include "VulkanDevice.h"
#include "Model.h"
#include "Shader.h"

class ResourceManager
{
private:
	VulkanDevice &device;
	VkCommandPool commandPool;
	VkQueue queue;
public:
	ResourceManager(VulkanDevice &device, VkQueue queue, VkCommandPool commandPool);
	~ResourceManager();

	std::unordered_map<std::string, Model*> models;
	std::unordered_map<std::string, Shader*> shaders;

	Model* loadModel(std::string id, std::string filename);
	Shader* loadShader(std::string id, std::string filename);

	Shader* getShader(std::string id);
	Model* getModel(std::string id);
};
