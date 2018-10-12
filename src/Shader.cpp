/*
* Vulkan Invaders - Shader module clas
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "Shader.h"

Shader::Shader(std::string id, std::string filename, VulkanDevice *device) : id(id), device(device)
{
	std::ifstream is(filename, std::ios::binary | std::ios::in | std::ios::ate);

	if (is.is_open()) {
		size_t size = is.tellg();
		is.seekg(0, std::ios::beg);
		char* shaderCode = new char[size];
		is.read(shaderCode, size);
		is.close();

		VkShaderModuleCreateInfo shaderModuleCI{};
		shaderModuleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCI.codeSize = size;
		shaderModuleCI.pCode = (uint32_t*)shaderCode;

		VK_CHECK_RESULT(vkCreateShaderModule((VkDevice)*device, &shaderModuleCI, NULL, &shaderModule));

		delete[] shaderCode;
	} else {
		std::cerr << "Error: Could not open shader file \"" << filename << "\"" << std::endl;
	}
}

Shader::~Shader()
{
	vkDestroyShaderModule((VkDevice)*device, shaderModule, nullptr);
}
