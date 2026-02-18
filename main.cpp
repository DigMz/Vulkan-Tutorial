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
