/*
* Vulkan Invaders - Renderer
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "Renderer.h"

#define VK_CHECK_RESULT(f)																				\
{																										\
	VkResult res = (f);																					\
	if (res != VK_SUCCESS)																				\
	{																									\
		std::cout << "Fatal : VkResult is " << res << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl; \
		assert(res == VK_SUCCESS);																		\
	}																									\
}

#if defined(VK_USE_PLATFORM_WIN32_KHR)
HWND Renderer::hWnd;
HINSTANCE Renderer::hInstance;
WNDPROC Renderer::wndProc;
#endif

VkBool32 debugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
	std::string prefix;
	char *message = (char*)malloc(strlen(pCallbackData->pMessage) + 5000);

	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
		prefix = "VERBOSE: ";
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
		prefix = "INFO : ";
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		prefix = "WARNING : ";
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
		prefix = "ERROR : ";
	}
	if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
		prefix = "GENERAL: ";
	}
	std::cout << prefix << "ID: " << pCallbackData->messageIdNumber << " NAME : " << pCallbackData->pMessageIdName << std::endl << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}


void Renderer::createSwapchain()
{
	VkSwapchainKHR oldSwapchain = swapchain;

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities));
	uint32_t presentModeCount;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr));
	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data()));

	VkExtent2D swapchainExtent{};
	if (surfaceCapabilities.currentExtent.width == (uint32_t)-1) {
		swapchainExtent.width = width;
		swapchainExtent.height = height;
	}
	else {
		swapchainExtent = surfaceCapabilities.currentExtent;
		width = swapchainExtent.width;
		height = swapchainExtent.height;
	}

	// Select a present mode
	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	if (!settings.vsync) {
		for (size_t i = 0; i < presentModeCount; i++) {
			if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
				swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
			if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
				swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}
	}

	uint32_t minImageCount = surfaceCapabilities.minImageCount + 1;
	if ((surfaceCapabilities.maxImageCount > 0) && (minImageCount > surfaceCapabilities.maxImageCount)) {
		minImageCount = surfaceCapabilities.maxImageCount;
	}

	VkSurfaceTransformFlagsKHR preTransform;
	if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else {
		preTransform = surfaceCapabilities.currentTransform;
	}

	VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
	};
	for (auto& compositeAlphaFlag : compositeAlphaFlags) {
		if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlag) {
			compositeAlpha = compositeAlphaFlag;
			break;
		};
	}

	VkSwapchainCreateInfoKHR swapchainCI{};
	swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCI.surface = surface;
	swapchainCI.minImageCount = minImageCount;
	swapchainCI.imageFormat = colorFormat;
	swapchainCI.imageColorSpace = colorSpace;
	swapchainCI.imageExtent = { swapchainExtent.width, swapchainExtent.height };
	swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
	swapchainCI.imageArrayLayers = 1;
	swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCI.presentMode = swapchainPresentMode;
	swapchainCI.oldSwapchain = oldSwapchain;
	swapchainCI.clipped = VK_TRUE;
	swapchainCI.compositeAlpha = compositeAlpha;
	VK_CHECK_RESULT(vkCreateSwapchainKHR(device, &swapchainCI, nullptr, &swapchain));

	if (oldSwapchain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(device, oldSwapchain, nullptr);
	}

	std::vector<VkImage> images;
	VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr));
	images.resize(swapchainImageCount);
	VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, images.data()));

	swapchainResources.resize(swapchainImageCount);
	for (uint32_t i = 0; i < swapchainImageCount; i++) {
		swapchainResources[i].image = images[i];
		VkImageViewCreateInfo colorAttachmentViewCI{};
		colorAttachmentViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		colorAttachmentViewCI.format = colorFormat;
		colorAttachmentViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		colorAttachmentViewCI.subresourceRange.baseMipLevel = 0;
		colorAttachmentViewCI.subresourceRange.levelCount = 1;
		colorAttachmentViewCI.subresourceRange.baseArrayLayer = 0;
		colorAttachmentViewCI.subresourceRange.layerCount = 1;
		colorAttachmentViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colorAttachmentViewCI.image = swapchainResources[i].image;
		VK_CHECK_RESULT(vkCreateImageView(device, &colorAttachmentViewCI, nullptr, &swapchainResources[i].view));
	}

	// Renderpass
	{
		std::array<VkAttachmentDescription, 1> attachmentDescriptions{};

		attachmentDescriptions[0].format = colorFormat;
		attachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		/*
		attachmentDescriptions[1].format = depthFormat;
		attachmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		*/

		VkAttachmentReference colorReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
		VkAttachmentReference depthReference = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

		VkSubpassDescription subpassDescription{};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorReference;
		//subpassDescription.pDepthStencilAttachment = &depthReference;

		std::array<VkSubpassDependency, 2> dependencies;

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo renderPassCI{};
		renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCI.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
		renderPassCI.pAttachments = attachmentDescriptions.data();
		renderPassCI.subpassCount = 1;
		renderPassCI.pSubpasses = &subpassDescription;
		renderPassCI.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassCI.pDependencies = dependencies.data();

		VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassCI, nullptr, &renderpass));
	}

	// Framebuffers
	{
		VkImageView attachments[1];
		//attachments[1] = depthStencil.view;

		VkFramebufferCreateInfo frameBufferCI{};
		frameBufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferCI.renderPass = renderpass;
		frameBufferCI.attachmentCount = 1;
		frameBufferCI.pAttachments = attachments;
		frameBufferCI.width = width;
		frameBufferCI.height = height;
		frameBufferCI.layers = 1;

		// Create frame buffers for every swap chain image
		for (uint32_t i = 0; i < swapchainImageCount; i++) {
			attachments[0] = swapchainResources[i].view;
			VK_CHECK_RESULT(vkCreateFramebuffer(device, &frameBufferCI, nullptr, &swapchainResources[i].framebuffer));
		}
	}
}

Renderer::Renderer()
{
	// Vulkan instance
	{
		VkApplicationInfo applicationInfo{};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pApplicationName = "VulkanInvaders";
		applicationInfo.pEngineName = "VulkanInvaders";
		applicationInfo.apiVersion = VK_API_VERSION_1_1;

		std::vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

#if defined(VK_USE_PLATFORM_WIN32_KHR)
		instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
		instanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
		instanceExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
		instanceExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_IOS_MVK)
		instanceExtensions.push_back(VK_MVK_IOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
		instanceExtensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#endif

		VkInstanceCreateInfo instanceCI{};
		instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCI.pApplicationInfo = &applicationInfo;

		VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI{};
		if (settings.validation) {
			instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

			char *validationLayerNames[] = { "VK_LAYER_LUNARG_standard_validation" };
			instanceCI.ppEnabledLayerNames = validationLayerNames;
			instanceCI.enabledLayerCount = 1;

			// VK_EXT_debug_utils
			debugUtilsMessengerCI.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugUtilsMessengerCI.messageSeverity =
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugUtilsMessengerCI.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugUtilsMessengerCI.pfnUserCallback = debugMessengerCallback;
			instanceCI.pNext = &debugUtilsMessengerCI;

		}
		instanceCI.ppEnabledExtensionNames = instanceExtensions.data();
		instanceCI.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
		VK_CHECK_RESULT(vkCreateInstance(&instanceCI, nullptr, &instance));

		if (settings.validation) {
			vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
			vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
			VK_CHECK_RESULT(vkCreateDebugUtilsMessengerEXT(instance, &debugUtilsMessengerCI, nullptr, &debugUtilsMessenger));
		}

	}

	// Vulkan instance function pointers
	{
		vkGetPhysicalDeviceSurfaceSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceSupportKHR"));
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
		vkGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
		vkGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR"));
	}

	// Vulkan device
	{
		// Use first Vulkan capable GPU
		uint32_t physicalDeviceCount = 0;
		VK_CHECK_RESULT(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));
		if (physicalDeviceCount == 0) {
			throw std::runtime_error("No Vulkan capable GPU available");
		}
		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		VK_CHECK_RESULT(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()));
		physicalDevice = physicalDevices[0];

		// Find a graphics queue family
		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

		bool graphicsQueueFamilyFound = false;
		for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++) {
			if (!graphicsQueueFamilyFound && (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
				graphicsQueueFamilyFound = true;
				graphicsQueueFamilyIndex = i;
			}
		}
		if (!graphicsQueueFamilyFound) {
			throw std::runtime_error("Could not find a queue family supporting graphics");
		}

		const float defaultQueuePriority(0.0f);
		VkDeviceQueueCreateInfo queueCI{};
		queueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCI.queueFamilyIndex = graphicsQueueFamilyIndex;
		queueCI.queueCount = 1;
		queueCI.pQueuePriorities = &defaultQueuePriority;

		// Required extensions
		std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		// Create logical device
		VkDeviceCreateInfo deviceCI{};
		deviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCI.queueCreateInfoCount = 1;
		deviceCI.pQueueCreateInfos = &queueCI;
		deviceCI.ppEnabledExtensionNames = deviceExtensions.data();
		deviceCI.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());

		VK_CHECK_RESULT(vkCreateDevice(physicalDevice, &deviceCI, nullptr, &device));

		// Get a graphics queue from the device
		vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);

		vulkanDevice = new VulkanDevice(physicalDevice, device);
	}

	// Vulkan device function pointers
	{
		vkCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(vkGetDeviceProcAddr(device, "vkCreateSwapchainKHR"));
		vkDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(vkGetDeviceProcAddr(device, "vkDestroySwapchainKHR"));
		vkGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(vkGetDeviceProcAddr(device, "vkGetSwapchainImagesKHR"));
		vkAcquireNextImageKHR = reinterpret_cast<PFN_vkAcquireNextImageKHR>(vkGetDeviceProcAddr(device, "vkAcquireNextImageKHR"));
		vkQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(vkGetDeviceProcAddr(device, "vkQueuePresentKHR"));
	}

	// Main window
#if defined(VK_USE_PLATFORM_WIN32_KHR)
	{
		if (settings.validation) {
			AllocConsole();
			AttachConsole(GetCurrentProcessId());
			FILE *stream;
			freopen_s(&stream, "CONOUT$", "w+", stdout);
			freopen_s(&stream, "CONOUT$", "w+", stderr);
			SetConsoleTitle(TEXT("VulkanInvaders Debug Window"));
		}

		WNDCLASSEX wndClass{};
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = Renderer::wndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = Renderer::hInstance;
		wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndClass.lpszClassName = "vulkanInvaders";
		wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
		assert(RegisterClassEx(&wndClass));

		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

		const int32_t screenWidth = GetSystemMetrics(SM_CXSCREEN);
		const int32_t screenHeight = GetSystemMetrics(SM_CYSCREEN);

		if (settings.fullscreen) {
			DEVMODE dmScreenSettings{};
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = screenWidth;
			dmScreenSettings.dmPelsHeight = screenHeight;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL) {
				dwExStyle = WS_EX_APPWINDOW;
				dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
			}
			else {
				(MessageBox(NULL, "Cloud not switch to fullscreen", "Error", MB_YESNO | MB_ICONEXCLAMATION) == IDYES);
			}
		}

		RECT windowRect;
		windowRect.left = 0L;
		windowRect.top = 0L;
		windowRect.right = settings.fullscreen ? (long)screenWidth : (long)width;
		windowRect.bottom = settings.fullscreen ? (long)screenHeight : (long)height;
		AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

		Renderer::hWnd = CreateWindowEx(0, "VulkanInvaders", "Vulkan Invaders", dwStyle, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, Renderer::hInstance, NULL);

		if (!settings.fullscreen) {
			uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
			uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
			SetWindowPos(Renderer::hWnd, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}

		ShowWindow(Renderer::hWnd, SW_SHOW);
		SetForegroundWindow(Renderer::hWnd);
		SetFocus(Renderer::hWnd);
	}
#endif

	// Surface
	{
		VkResult result;
#if defined(VK_USE_PLATFORM_WIN32_KHR)
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.hinstance = Renderer::hInstance;
		surfaceCreateInfo.hwnd = Renderer::hWnd;
		result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
		VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = {};
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.window = window;
		result = vkCreateAndroidSurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface);
#elif defined(VK_USE_PLATFORM_IOS_MVK)
		VkIOSSurfaceCreateInfoMVK surfaceCreateInfo = {};
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK;
		surfaceCreateInfo.pNext = NULL;
		surfaceCreateInfo.pView = view;
		result = vkCreateIOSSurfaceMVK(instance, &surfaceCreateInfo, nullptr, &surface);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
		VkMacOSSurfaceCreateInfoMVK surfaceCreateInfo = {};
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
		surfaceCreateInfo.pNext = NULL;
		surfaceCreateInfo.pView = view;
		result = vkCreateMacOSSurfaceMVK(instance, &surfaceCreateInfo, NULL, &surface);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
		VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo = {};
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.display = display;
		surfaceCreateInfo.surface = window;
		result = vkCreateWaylandSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
		VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.connection = connection;
		surfaceCreateInfo.window = window;
		result = vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
#endif
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Could not create a surface");
		}
	}

	// Swapchain (only initialization, no creation)
	{
		// Find a presentation queue family
		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

		bool presentQueueFamilyFound = false;
		for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++) {
			VkBool32 supportsPresent;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportsPresent);
			if (!presentQueueFamilyFound && supportsPresent) {
				presentQueueFamilyFound = true;
				presentQueueFamilyIndex = i;
			}
		}
		if (!presentQueueFamilyFound) {
			throw std::runtime_error("Could not find a queue family supporting presentation");
		}
		if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
			throw std::runtime_error("Separate graphics and present queues are not supported yet!");
		}

		// Find supported and preferred color space and format
		uint32_t formatCount;
		VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr));
		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data()));
		if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED)) {
			colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
			colorSpace = surfaceFormats[0].colorSpace;
		}
		else {
			bool found_B8G8R8A8_UNORM = false;
			for (auto&& surfaceFormat : surfaceFormats) {
				if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM) {
					colorFormat = surfaceFormat.format;
					colorSpace = surfaceFormat.colorSpace;
					found_B8G8R8A8_UNORM = true;
					break;
				}
			}
			if (!found_B8G8R8A8_UNORM) {
				colorFormat = surfaceFormats[0].format;
				colorSpace = surfaceFormats[0].colorSpace;
			}
		}

		createSwapchain();
	}

	// Base resources
	{
		VkCommandPoolCreateInfo commandPoolCI{};
		commandPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCI.queueFamilyIndex = graphicsQueueFamilyIndex;
		VK_CHECK_RESULT(vkCreateCommandPool(device, &commandPoolCI, nullptr, &commandPool));
	}

	// Fixed per-frame resources
	{
		frameResources.resize(swapchainImageCount);
		for (auto &frame : frameResources) {
			VkSemaphoreCreateInfo semaphoreCI{};
			semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreCI, nullptr, &frame.drawCompleteSemaphore));
			VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreCI, nullptr, &frame.imageAcquiredSemaphore));

			VkFenceCreateInfo fenceCI{};
			fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			VK_CHECK_RESULT(vkCreateFence(device, &fenceCI, nullptr, &frame.waitFence));

			VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.commandPool = commandPool;
			commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocateInfo.commandBufferCount = 1;
			VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &frame.commandBuffer));
		}
	}

	updateCommandBuffers();
}

Renderer::~Renderer()
{
	vkDeviceWaitIdle(device);
	vkDestroyDevice(device, nullptr);
	vkDestroyInstance(instance, nullptr);
	delete vulkanDevice;
}

void Renderer::updateCommandBuffers()
{
	VkClearValue clearValues[2];
	clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderpass;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = width;
	renderPassBeginInfo.renderArea.extent.height = height;
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValues;

	VkCommandBufferBeginInfo commandBufferBeginInfo{};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	for (auto i = 0; i < frameResources.size(); ++i) {
		// Set target frame buffer
		renderPassBeginInfo.framebuffer = swapchainResources[i].framebuffer;

		VK_CHECK_RESULT(vkBeginCommandBuffer(frameResources[i].commandBuffer, &commandBufferBeginInfo));

		vkCmdBeginRenderPass(frameResources[i].commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Update dynamic viewport state
		VkViewport viewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
		vkCmdSetViewport(frameResources[i].commandBuffer, 0, 1, &viewport);

		// Update dynamic scissor state
		VkRect2D scissor = { { 0, 0 },{ width, height } };
		vkCmdSetScissor(frameResources[i].commandBuffer, 0, 1, &scissor);

		vkCmdEndRenderPass(frameResources[i].commandBuffer);

		VK_CHECK_RESULT(vkEndCommandBuffer(frameResources[i].commandBuffer));
	}
}

void Renderer::draw()
{
	vkWaitForFences(device, 1, &frameResources[currentFrame].waitFence, VK_TRUE, UINT64_MAX);
	vkResetFences(device, 1, &frameResources[currentFrame].waitFence);

	VK_CHECK_RESULT(vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, frameResources[currentFrame].imageAcquiredSemaphore, VK_NULL_HANDLE, &currentImage));

	const VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pWaitDstStageMask = &pipelineStageFlags;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &frameResources[currentImage].imageAcquiredSemaphore;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &frameResources[currentFrame].commandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &frameResources[currentImage].drawCompleteSemaphore;
	VK_CHECK_RESULT(vkQueueSubmit(graphicsQueue, 1, &submitInfo, frameResources[currentFrame].waitFence));

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.pImageIndices = &currentImage;
	presentInfo.pWaitSemaphores = &frameResources[currentFrame].drawCompleteSemaphore;
	presentInfo.waitSemaphoreCount = 1;
	VK_CHECK_RESULT(vkQueuePresentKHR(graphicsQueue, &presentInfo));

	currentFrame += 1;
	currentFrame %= static_cast<uint32_t>(frameResources.size());
}
