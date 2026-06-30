#pragma once

#include "vulkan/vulkan.hpp"
#include <array>
#include <cstddef>
#include <iostream>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

#include <cstdint>
#include <vector>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.hpp>
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#	include <vulkan/vulkan_raii.hpp>
#else
import vulkan.hpp;
#endif
#include <GLFW/glfw3.h>
#define STD_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const uint32_t WIDTH  = 800;
const uint32_t HEIGHT = 600;
constexpr int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<char const*> validationLayers = {
  "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif // NDEBUG

struct Vertex {
  glm::vec2 pos;
  glm::vec3 color;

  static vk::VertexInputBindingDescription getBindingDescription() {
    return {
      .binding   = 0,
      .stride    = sizeof(Vertex),
      .inputRate = vk::VertexInputRate::eVertex,
    };
  }

  static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {
    return {{
      {.location = 0, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(Vertex, pos)},
      {.location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(Vertex, color)}
    }};
  }
};

struct UniformBufferObject {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;
};

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},
};
const std::vector<uint16_t> indices = {
  0, 1, 2, 2, 3, 0
};

class Application
{
public:
	void run();

private:
	GLFWwindow                           *window        = nullptr;
  vk::raii::Context                    context;
  vk::raii::Instance                   instance       = nullptr;
  vk::raii::DebugUtilsMessengerEXT     debugMessenger = nullptr;
  vk::raii::SurfaceKHR                 surface        = nullptr;
  vk::raii::PhysicalDevice             physicalDevice = nullptr;
  vk::raii::Device                     device         = nullptr;
  vk::raii::Queue                      queue          = nullptr;
  uint32_t                             queueIndex     = ~0;
  vk::raii::SwapchainKHR               swapChain      = nullptr;
  std::vector<vk::Image>               swapChainImages;
  vk::SurfaceFormatKHR                 swapChainSurfaceFormat;
  vk::Extent2D                         swapChainExtent;
  std::vector<vk::raii::ImageView>     swapChainImageViews;

  vk::raii::DescriptorSetLayout        descriptorSetLayout = nullptr;
  vk::raii::PipelineLayout             pipelineLayout      = nullptr;
  vk::raii::Pipeline                   graphicsPipeline    = nullptr;

  vk::raii::Image                      textureImage       = nullptr;
  vk::raii::DeviceMemory               textureImageMemory = nullptr;
  vk::raii::ImageView                  textureImageView   = nullptr;
  vk::raii::Sampler                    textureSampler     = nullptr;

  vk::raii::Buffer                     vertexBuffer       = nullptr;
  vk::raii::DeviceMemory               vertexBufferMemory = nullptr;
  vk::raii::Buffer                     indexBuffer        = nullptr;
  vk::raii::DeviceMemory               indexBufferMemory  = nullptr;

  std::vector<vk::raii::Buffer>        uniformBuffers;
  std::vector<vk::raii::DeviceMemory>  uniformBuffersMemory;
  std::vector<void *>                  uniformBuffersMapped;

  vk::raii::DescriptorPool             descriptorPool = nullptr;
  std::vector<vk::raii::DescriptorSet> descriptorSets;

  vk::raii::CommandPool                commandPool      = nullptr;
  std::vector<vk::raii::CommandBuffer> commandBuffers;

  std::vector<vk::raii::Semaphore>     presentCompleteSemaphores;
  std::vector<vk::raii::Semaphore>     renderFinishedSemaphores;
  std::vector<vk::raii::Fence>         inFlightFences;
  uint32_t                             frameIndex         = 0;
  bool                                 framebufferResized = false;

  std::vector<const char *> requiredDeviceExtension = {
    vk::KHRSwapchainExtensionName
  };

  static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT       severity,
                                                        vk::DebugUtilsMessageTypeFlagsEXT              type,
                                                        const vk::DebugUtilsMessengerCallbackDataEXT * pCallbackData,
                                                        void *                                         pUserData)
  {
    std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;

    return vk::False;
  };

	void initWindow();

  static void framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto app = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
  }

	void initVulkan();
  void createInstance();
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createSwapChain();
  void createImageViews();
  void createDescriptorSetLayout();
  void createGraphicsPipeline();
  void createCommandPool();
  void createTextureImage();
  std::pair<vk::raii::Image, vk::raii::DeviceMemory> createImage(
    uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties
  );
  void transitionImageLayout(vk::raii::CommandBuffer &commandBuffer, const vk::raii::Image &image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
  void copyBufferToImage(vk::raii::CommandBuffer &commandBuffer, const vk::raii::Buffer &buffer, vk::raii::Image &image, uint32_t width, uint32_t height);
  std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
  vk::raii::CommandBuffer beginSingleTimeCommands();
  void endSingleTimeCommands(vk::raii::CommandBuffer &&commandBuffer);
  void copyBuffer(vk::raii::Buffer & srcBuffer, vk::raii::Buffer & dstBuffer, vk::DeviceSize size);
  void createVertexBuffer();
  void createIndexBuffer();
  void createUniformBuffers();
  void createDescriptorPool();
  void createDescriptorSets();
  uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
  void createCommandBuffers();

  void recordCommandBuffer(uint32_t imageIndex);
  void transition_image_layout(
    uint32_t                imageIndex,
    vk::ImageLayout         old_layout,
    vk::ImageLayout         new_layout,
    vk::AccessFlags2        src_access_mask,
    vk::AccessFlags2        dst_access_mask,
    vk::PipelineStageFlags2 src_stage_mask,
    vk::PipelineStageFlags2 dst_stage_mask
  );

  void createSyncObjects();
  void updateUniformBuffer(uint32_t currentImage);
  void drawFrame();

  [[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code) const {
    vk::ShaderModuleCreateInfo createInfo {
      .codeSize = code.size() * sizeof(char),
      .pCode = reinterpret_cast<const uint32_t*>(code.data())
    };
    vk::raii::ShaderModule shaderModule { device, createInfo };
    return shaderModule;
  }

  uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const &surfaceCapabilites);
  vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const &availableFormats);
  vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const &availablePresentModes);
  vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const &capabilities);

  std::vector<const char*> getRequiredInstanceExtensions();

	void mainLoop();
	void cleanup();

  void cleanupSwapChain();
  void recreateSwapChain();
};
