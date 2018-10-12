/*
* Vulkan Invaders - Shader module clas
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include <string>
#include <fstream>

#include "vulkan/vulkan.h"
#include "VulkanUtils.h"
#include "VulkanDevice.h"

class Shader
{
private:
	VulkanDevice *device;
	VkShaderModule shaderModule;
public:
	std::string id;
	Shader(std::string id, std::string filename, VulkanDevice *device);
	~Shader();

	operator VkShaderModule() { return shaderModule; };
};

