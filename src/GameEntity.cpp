/*
* Vulkan Invaders - Base game entity class
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "GameEntity.h"

GameEntity::GameEntity()
{
}

GameEntity::~GameEntity()
{
}

bool GameEntity::hitTest(GameEntity *target)
{
	return ((position.x >= target->position.x - target->size.x) && (position.x <= target->position.x + target->size.x) && (position.y >= target->position.y - target->size.y) && (position.y <= target->position.y + target->size.y));
}
