/*
* Vulkan Invaders - Entity manager class
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "EntityManager.h"

EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
}

void EntityManager::addEntity(std::string id, GameEntity *entity)
{
	entities.emplace(id, entity);
}

GameEntity* EntityManager::getEntity(std::string id)
{
	auto it = entities.find(id);
	assert(it != entities.end());
	return it->second;
}
