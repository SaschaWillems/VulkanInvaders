/*
* Vulkan Invaders - Main game class
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "VulkanInvaders.h"

VulkanInvaders::VulkanInvaders()
{
	renderer = new Renderer();
}

VulkanInvaders::~VulkanInvaders()
{
	delete renderer;
}

void VulkanInvaders::mainLoop()
{
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
		}
		if (!IsIconic(Renderer::hWnd) && !quit) {
			renderer->draw();
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
