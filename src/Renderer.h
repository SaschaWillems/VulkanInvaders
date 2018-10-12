/*
* Vulkan Invaders - Renderer
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include <vector>
#include <array>
#include <iostream>
#include <assert.h>
#include <string>
#include <chrono>
#include <glm/glm.hpp>
#include "vulkan/vulkan.h"

#include "ResourceManager.h"
#include "EntityManager.h"
#include "Camera.h"
#include "Model.h"
#include "VulkanUtils.h"
#include "VulkanDevice.h"

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

class Renderer
{
private:
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue graphicsQueue;
	VkCommandPool commandPool;
	VkRenderPass renderpass;
	VkPipelineCache pipelineCache;

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

	uint32_t width = 224 * 2;
	uint32_t height = 256 * 2;

	// Dummy
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
	VulkanDeviceBuffer sceneUniformBuffer;

	Camera camera;

	struct Settings {
		bool validation = true;
		bool fullscreen = false;
		bool vsync = false;
	} settings;

	void createSwapchain();
public:
	Renderer();
	~Renderer();

	ResourceManager *resourceManager;
	EntityManager *entityManager;
	VulkanDevice *vulkanDevice;

#if defined(VK_USE_PLATFORM_WIN32_KHR)
	static HWND hWnd;
	static HINSTANCE hInstance;
	static WNDPROC wndProc;
#endif

	float lastFrameTime = 0.0f;

	void updateCommandBuffers();
	void createPipelines();

	void draw();

	VulkanDevice *getDevice() { return vulkanDevice; }
	VkQueue getQueue() { return graphicsQueue; }
	VkCommandPool getCommandPool() { return commandPool; }
	VkDescriptorPool getDescriptorPool() { return descriptorPool;  }
};

