#include <vector>
#include <array>
#include <iostream>
#include <assert.h>
#include <string>
#include "vulkan/vulkan.h"

#pragma once

struct SwapchainResource {
	VkImage image;
	VkImageView view;
	VkFramebuffer framebuffer;
};

struct FrameResource {
	VkSemaphore imageAcquiredSemaphore;
	VkSemaphore drawCompleteSemaphore;
	VkFence waitFence;
	VkCommandBuffer commandBuffer;
};

class VulkanInvaders
{
private:
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue graphicsQueue;
	VkCommandPool commandPool;
	VkRenderPass renderpass;

	VkSurfaceKHR surface;

	PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
	PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
	PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
	PFN_vkQueuePresentKHR vkQueuePresentKHR;
	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;

	VkDebugUtilsMessengerEXT debugUtilsMessenger;

	uint32_t graphicsQueueFamilyIndex;
	uint32_t presentQueueFamilyIndex;

	VkFormat colorFormat;
	VkColorSpaceKHR colorSpace;
	uint32_t swapchainImageCount;
	std::vector<SwapchainResource> swapchainResources;
	uint32_t currentImage = 0;
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;

	uint32_t currentFrame = 0;
	std::vector<FrameResource> frameResources;

	uint32_t width = 1280;
	uint32_t height = 720;

	struct Settings {
		bool validation = true;
		bool fullscreen = false;
		bool vsync = false;
	} settings;

	void createSwapchain();
	void updateCommandBuffers();
	void draw();

public:

#if defined(VK_USE_PLATFORM_WIN32_KHR)
	static HWND hWnd;
	static HINSTANCE hInstance;
	static WNDPROC wndProc;
#endif

	bool quit = false;

	VulkanInvaders();
	~VulkanInvaders();

	void mainLoop();

#if defined(VK_USE_PLATFORM_WIN32_KHR)
	void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
};

