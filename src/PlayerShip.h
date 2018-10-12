/*
* Vulkan Invaders - Player ship entity class
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once
#include "GameEntity.h"

#include <glm/glm.hpp>
#include "vulkan/vulkan.h"
#include "VulkanDevice.h"
#include "Model.h"

class PlayerShip : public GameEntity
{
private:
	struct Matrices {
		glm::mat4 model;
	} uniformData;
public:
	bool updated = true;
	float firingTimer = 0.0f;
	Model* model;
	PlayerShip();
	PlayerShip(Model *model);
	~PlayerShip();
	void allocateResources(VulkanDevice *device, VkDescriptorPool descriptorPool);
	void update(float deltatime);
	void updateUniformdata();
	void draw(VkCommandBuffer commandBuffer);
};