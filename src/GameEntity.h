/*
* Vulkan Invaders - Base game entity class
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include <glm/glm.hpp>
#include "vulkan/vulkan.h"
#include "VulkanDevice.h"

class GameEntity
{
protected:
	glm::vec3 speed;
	VulkanDeviceBuffer ubo;
public:
	VkDescriptorSet descriptorSet;
	VkDescriptorSetLayout descriptorSetLayout;

	glm::vec3 position;
	glm::vec3 size;

	GameEntity *owner;

	GameEntity();
	virtual ~GameEntity();

	bool hitTest(GameEntity *target);

	virtual void update(float timestamp) = 0;
	virtual void draw(VkCommandBuffer commandBuffer) = 0;
};

