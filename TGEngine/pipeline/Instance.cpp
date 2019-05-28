#include "Instance.hpp"

VkResult lastResult;
VkInstance instance;
VkDebugUtilsMessengerEXT debugMessager;

void createInstance(std::vector<const char*> layersToEnable, std::vector<const char*> extensionsToEnable) {

	VkApplicationInfo app_info = { 
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		properties->getStringOrDefault("app_name", "TGEngine"),
		(uint32_t)properties->getInt("version"),
	    "TGEngine",
	    TGE_VERSION,
		VK_API_VERSION_1_1
	};

	uint32_t count;
	//Validation for the instance layers
	std::vector<const char*> enabledLayerNames;
	if (layersToEnable.size() > 0) {
		lastResult = vkEnumerateInstanceLayerProperties(&count, nullptr);
		HANDEL(lastResult)
		std::vector<VkLayerProperties> usableLayerNames(count);
		lastResult = vkEnumerateInstanceLayerProperties(&count, usableLayerNames.data());
		HANDEL(lastResult)
		for each(VkLayerProperties layer in usableLayerNames) {
			OUT_LV_DEBUG("Available " << layer.layerName)
			for each(const char* name in layersToEnable) {
				if (strcmp(layer.layerName, name) == 0) {
					enabledLayerNames.push_back(name);
					OUT_LV_DEBUG("Activate Layer: " << name)
					break;
				}
			}
		}
	}

#ifdef DEBUG
	extensionsToEnable.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    #ifdef _WIN32
	extensionsToEnable.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	extensionsToEnable.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    #endif

	//Validation for the intance extensions
	std::vector<const char*> enabledExtensionNames;
	if (extensionsToEnable.size() > 0) {
		lastResult = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
		HANDEL(lastResult)
		std::vector<VkExtensionProperties> usableExtensionNames(count);
		lastResult = vkEnumerateInstanceExtensionProperties(nullptr, &count, usableExtensionNames.data());
		HANDEL(lastResult)
		for each(VkExtensionProperties extension in usableExtensionNames) {	
			OUT_LV_DEBUG("Available " << extension.extensionName)
			for each(const char* name in extensionsToEnable) {
				if (strcmp(extension.extensionName, name) == 0) {
					enabledExtensionNames.push_back(name);
					OUT_LV_DEBUG("Active " << name)
					break;
				}
			}
		}
	}
	
	//Create Instance
	VkInstanceCreateInfo instance_create_info = {
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
	    nullptr,
	    0,
	    &app_info,
		(uint32_t)enabledLayerNames.size(),
	    enabledLayerNames.data(),
		(uint32_t)enabledExtensionNames.size(),
	    enabledExtensionNames.data()
	};
	lastResult = vkCreateInstance(&instance_create_info, nullptr, &instance);
	HANDEL(lastResult)

#ifdef DEBUG
	/*
	 * Create a debug messenger
	 */
	VkDebugUtilsMessengerCreateInfoEXT utilMessagerCreateInfo;
	utilMessagerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	utilMessagerCreateInfo.flags = 0;
	utilMessagerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	utilMessagerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	utilMessagerCreateInfo.pfnUserCallback = debugCallback;
	utilMessagerCreateInfo.pUserData = 0;
	utilMessagerCreateInfo.pNext = 0;

	PFN_vkCreateDebugUtilsMessengerEXT createDebugReportCallback = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	OUT_LV_DEBUG(createDebugReportCallback)
	if (!createDebugReportCallback) {
		TGERROR(TG_ERR_DB_DBMESSAGER_NOT_VALID)
	}
	lastResult = createDebugReportCallback(instance, &utilMessagerCreateInfo, nullptr, &debugMessager);
	HANDEL(lastResult)
#endif
}

#ifdef DEBUG
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT                  messageType,
	const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
	void *pUserData) {

	switch (messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		std::cout << "VERBOS [";
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		std::cout << "INFO [";
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		std::cout << "WARNING [";
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		std::cout << "ERROR [";
		break;
	default:
		break;
	}

	switch (messageType)
	{
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
		std::cout << "GENERAL]: ";
		break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
		std::cout << "PERFORMANCE]: ";
		break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
		std::cout << "VALIDATION]: ";
		break;
	default:
		break;
	}

	std::cout << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}
#endif // DEBUG

void destroyInstance() {
#ifdef DEBUG
	PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugReportCallback = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (!destroyDebugReportCallback) {
		TGERROR(TG_ERR_DB_DBMESSAGER_NOT_VALID)
	}
	destroyDebugReportCallback(instance, debugMessager, nullptr);
#endif
	vkDestroyInstance(instance, nullptr);
}