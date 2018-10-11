/*
* Vulkan Invaders - Entity manager class
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include <unordered_map>
#include "GameEntity.h"

class EntityManager
{
public:
	std::unordered_map<std::string, GameEntity*> entities;
	EntityManager();
	~EntityManager();
	void addEntity(std::string id, GameEntity *entity);
	GameEntity* getEntity(std::string id);
};

