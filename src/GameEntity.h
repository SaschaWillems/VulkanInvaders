/*
* Vulkan Invaders - Base game entity class
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include "vulkan/vulkan.h"
#include "VulkanDevice.h"

class GameEntity
{
protected:
	VulkanDeviceBuffer ubo;
public:
	VkDescriptorSet descriptorSet;
	VkDescriptorSetLayout descriptorSetLayout;

	GameEntity();
	~GameEntity();

	virtual void update(float timestamp) = 0;
	virtual void draw(VkCommandBuffer commandBuffer) = 0;
};

