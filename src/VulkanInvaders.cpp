/*
* Vulkan Invaders - Main game class
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "VulkanInvaders.h"

void VulkanInvaders::loadAssets()
{
	resourceManager->loadModel("player_ship", "data/models/playership.obj");
	resourceManager->loadModel("enemy_01", "data/models/enemy01.obj");
	resourceManager->loadModel("projectile", "data/models/projectile.obj");
	resourceManager->loadShader("player_ship_vs", "data/shaders/playership.vert.spv");
	resourceManager->loadShader("player_ship_fs", "data/shaders/playership.frag.spv");
}

VulkanInvaders::VulkanInvaders()
{
	entityManager = new EntityManager();
	renderer = new Renderer();
	resourceManager = new ResourceManager(renderer->getDevice(), renderer->getQueue(), renderer->getCommandPool());

	renderer->resourceManager = resourceManager;
	renderer->entityManager = entityManager;

	loadAssets();

	renderer->createPipelines();

	PlayerShip *playerShip = new PlayerShip(resourceManager->getModel("player_ship"));
	playerShip->allocateResources(renderer->getDevice(), renderer->getDescriptorPool());

	entityManager->addEntity("player_ship", playerShip);

	glm::vec3 origin = glm::vec3(-10.0f, -10.0f, 0.0f);
	for (int32_t x = 0; x < 5; x++) {
		for (int32_t y = 0; y < 5; y++) {
			EnemyShip *enemyShip = new EnemyShip(resourceManager->getModel("enemy_01"), origin + glm::vec3((float)x * 3.0f, (float)y * 2.0f, 0.0f));
			enemyShip->allocateResources(renderer->getDevice(), renderer->getDescriptorPool());
			entityManager->addEntity("enemy_" + std::to_string(x) + "_" + std::to_string(y), enemyShip);
		}
	}

	renderer->updateCommandBuffers();
}

VulkanInvaders::~VulkanInvaders()
{
	vkDeviceWaitIdle((VkDevice)*renderer->getDevice());
	delete entityManager;
	delete resourceManager;
	delete renderer;
}

void VulkanInvaders::mainLoop()
{
	bool updateCB = false;
	while (!quit) {
#if defined(VK_USE_PLATFORM_WIN32_KHR)
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT) {
				quit = true;
				break;
			}
			switch (msg.message) 
			{
				case WM_KEYDOWN:
					switch (msg.wParam)
					{
					case VK_ESCAPE:
						PostQuitMessage(0);
						break;
					case VK_RETURN:
						PlayerShip *playerShip = dynamic_cast<PlayerShip*>(entityManager->getEntity("player_ship"));
						if (playerShip->firingTimer <= 0.0f) {
							playerShip->firingTimer = 25.0f;
							Projectile *projectile = new Projectile(resourceManager->getModel("projectile"), playerShip->position, glm::vec3(0.0f, -1.0f, 0.0f));
							projectile->allocateResources(renderer->getDevice(), renderer->getDescriptorPool());
							projectile->owner = playerShip;
							entityManager->addEntity("player_projectile" + std::to_string(entityManager->entities.size()), projectile);
							updateCB = true;
						}
					}
			}
		}
		if (!IsIconic(Renderer::hWnd) && !quit) {
			renderer->draw();
		}
		// TODO: Just for testing
		std::unordered_map<std::string, GameEntity*> deleteList;
		for (auto entity : entityManager->entities) {
			if (dynamic_cast<const Projectile*>(entity.second) != nullptr) {
				for (auto target : entityManager->entities) {
					if ((target != entity) && (target.second != entity.second->owner)) {
						GameEntity *src = entity.second;
						GameEntity *dst = target.second;
						if (src->hitTest(dst)) {
							deleteList.emplace(entity);
							deleteList.emplace(target);
							PlayerShip *playerShip = dynamic_cast<PlayerShip*>(entityManager->getEntity("player_ship"));
							playerShip->firingTimer = 0.0f;
							updateCB = true;
							break;
						}
					}
				}
			}
		}
		// TODO: Nice and proper sync, per-frame entity resources
		if (updateCB) {
			vkQueueWaitIdle(renderer->getQueue());
			for (auto entity : deleteList) {
				entityManager->removeEntity(entity.first);
				delete entity.second;
			}
			renderer->updateCommandBuffers();
			updateCB = false;
		}
		for (auto &entity : entityManager->entities) {
			entity.second->update(renderer->lastFrameTime);
		}
#endif
	}
}

#if defined(VK_USE_PLATFORM_WIN32_KHR)
void VulkanInvaders::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		break;
	}
}
#endif
