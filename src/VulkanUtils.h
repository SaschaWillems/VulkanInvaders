#define VK_CHECK_RESULT(f) {																						\
	if (f != VK_SUCCESS) {std::cerr << "Fatal : VkResult is " << f << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl;	\
		assert(f == VK_SUCCESS);																					\
	}																												\
}