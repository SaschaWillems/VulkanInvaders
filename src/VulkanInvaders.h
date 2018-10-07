/*
* Vulkan Invaders - Main game class
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "Renderer.h"

#pragma once

class VulkanInvaders
{
private:
	Renderer *renderer;
public:
	bool quit = false;

	VulkanInvaders();
	~VulkanInvaders();

	void mainLoop();

#if defined(VK_USE_PLATFORM_WIN32_KHR)
	void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
};

