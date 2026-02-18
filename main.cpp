#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

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
  GLFWwindow* window;

  VkInstance instance;

  // Create GLFW Window
  void initWindow() {
    // Initialize GLFW
    glfwInit();

    // Set to not use OpenGL API, Vulkan will be used
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // Resizability will be on the backburner for now
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  }
  void initVulkan() {
    createInstance();
  }

  // ######## USED IN initVulkan ########
  void createInstance() {
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

    // Count gotten later
    uint32_t glfwExtensionCount = 0;
    // Holds GLFW Extensions to be used in program
    const char** glfwExtensions;
    // Function returns Extensions to glfwExtensions and sets the extension count
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // Create Info for extensions filled out
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    // Used to determine global validation layers, will be used later, blank now
    createInfo.enabledLayerCount = 0;

    // Call creation function, throwing error if not successful
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
      throw std::runtime_error("failed to create instance!");
    }
  }

  void mainLoop() {
    // Loops until GLFW calls that the window should close
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
    }
  }

  void cleanup() {
    // Destroy window
    glfwDestroyWindow(window);

    // Terminate GLFW
    glfwTerminate();
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
