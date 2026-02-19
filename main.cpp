#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <map>
#include <optional>

// Window WIDTH and HEIGHT
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

// Lists validationLayers
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

// Macro used in compilation to enable or disable validationLayers
#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif

// Proxy extension function to create a VkDebugUtilsMessengerEXT object
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
  // Takes function from vkGetInstanceProcAddr and pushes forward its parameters, or return error
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
      return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
      return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

// Proxy extension function to destroy a VkDebugUtilsMessengerEXT object
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

// Helper struct to manage graphicsFamily indices
struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;

  bool isComplete() {
    return graphicsFamily.has_value();
  }
};

// Application Class
class HelloTriangleApplication {
public:
  // Main function to run applciation
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  GLFWwindow* window; // window used by Vulkan

  VkInstance instance; // instance of Vulkan
  VkDebugUtilsMessengerEXT debugMessenger; // Vulkan debugMessenger
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

  // Create GLFW Window
  void initWindow() {
    // Initialize GLFW
    glfwInit();

    // Set to not use OpenGL API, Vulkan will be used
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // Resizability will be on the backburner for now
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // doniw eht etaerC
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  }
  
  void initVulkan() {
    createInstance();
    setupDebugMessenger();
    pickPhysicalDevice();
  }

  void mainLoop() {
    // Loops until GLFW calls that the window should close
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
    }
  }

  void cleanup() {
    // If validationLayers are on, destory the debugMessenger
    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    // Destroy the Vulkan Instance
    vkDestroyInstance(instance, nullptr);

    // Destroy window
    glfwDestroyWindow(window);

    // Terminate GLFW
    glfwTerminate();
  }

  // ######## USED IN initVulkan ########
  void createInstance() {
    // Checks if validation layer is available when used, throwing error if not present
    if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error("validation layers requested, but not available!");
    }

    // Fillout App Info
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; // sType
    appInfo.pApplicationName = "Hello Triangle"; // Name of Application
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); // Version count of Application
    appInfo.pEngineName = "No Engine"; // Engine Name of Application
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); // Engine Version
    appInfo.apiVersion = VK_API_VERSION_1_0; // Version of Vulkan API being used

    // Fillout Creation Info for VkInstance
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; // sType
    createInfo.pApplicationInfo = &appInfo; // Application Info | from appInfo

    // Create Info for extensions filled out
    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // Create Info for debugMessenger
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
      // Sets layer count and names
      createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();

      // adds debugMessenger create info to pNext
      populateDebugMessengerCreateInfo(debugCreateInfo);
      createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
      // Case with no validationLayers
      createInfo.enabledLayerCount = 0;

      createInfo.pNext = nullptr;
    }

    // Call creation function, throwing error if not successful
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
      throw std::runtime_error("failed to create instance!");
    }
  }

  // Function fills debugMessenger createInfo, used in setupDebugMessenger
  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    }

  void setupDebugMessenger() {
    // If no validationLayers quit early
    if (!enableValidationLayers) {return;}
    
    // Make and fill createInfo for debugMessenger
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    // Create the debugMessenger, throw if error
    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
      throw std::runtime_error("failed to set up debug messenger!");
    }
  }

  void pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    // Get device count with Vulkan API
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
      throw std::runtime_error("failed to find GPU with Vulkan Support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    // Fill devices vector to get list of devices with vulkan support
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    std::multimap<int, VkPhysicalDevice> candidates;


    for (const auto& device : devices) {
        int score = rateDeviceSuitability(device);
        candidates.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0) {
        physicalDevice = candidates.rbegin()->second;
    } else {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
  }

  int rateDeviceSuitability(VkPhysicalDevice device) {
    // Variables hold device info
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    // Getting device info through API
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    // ## Scoring section
    QueueFamilyIndices indices = findQueueFamilies(device);
    if (!indices.isComplete()) {
      return 0;
    }
    int score = 0;

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      score += 1000;
    }

    // Max possible texture size affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // Application can't function without geometry shaders
    if (!deviceFeatures.geometryShader) {
      return 0;
    }

    return score;
  }

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    // get count with API
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    // use count for vector size
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    // fill vector with API
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    // Loop through queueFamilies, saving index if valid, returning indices
    for (const auto& queueFamily : queueFamilies) {
      // Bit manipulation for queueFlag check
      if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indices.graphicsFamily = i;
        if (indices.isComplete()) {
          break;
        }
      }
      i++;
    }

    return indices;
  }

  // ########## USED IN createInstance ##########

  // Gets list of required extensions, adding debug utils if 
  //  validation layers are enabled
  std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
  }

  bool checkValidationLayerSupport() {
    // Holds layer count
    uint32_t layerCount;
    // Gets layer count
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    // Makes vector holding availableLayers of count layerCount
    std::vector<VkLayerProperties> availableLayers(layerCount);
    // Fills in availableLayers' data and sets the layer count
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // Goes through the validationLayers
    // Returns false if a layer isn't supported
    for (const char* layerName : validationLayers) {
      bool layerFound = false;

      // Checks if validation layer is within list of availableLayers
      for (const auto& layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
            layerFound = true;
            break;
        }
      }

      if (!layerFound) {
        return false;
      }
    }

    return true;
  }

  // ######### createInstance FUNCTIONS END ##########
  // ######### initVulkan     FUNCTIONS END ##########
  
  // Callback for debug messages
  static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
  }

};

int main() {
  HelloTriangleApplication app;

  try {
      app.run();
  } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
