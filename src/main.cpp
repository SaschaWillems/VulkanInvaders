/*
* Vulkan Invaders
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#ifdef _WIN32
#pragma comment(linker, "/subsystem:windows")
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#endif

#include <vector>
#include <iostream>
#include <assert.h>
#include "vulkan/vulkan.h"

#include "VulkanInvaders.h"

VulkanInvaders *game;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	game->handleMessages(hWnd, uMsg, wParam, lParam);
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) 
{
	VulkanInvaders::hInstance = hInstance;
	VulkanInvaders::wndProc = WndProc;
	game = new VulkanInvaders();
	game->mainLoop();
	delete(game);
	return 0;
}