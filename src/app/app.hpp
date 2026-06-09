#pragma once

#include "vulkan/vulkan.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>

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

const uint32_t WIDTH  = 800;
const uint32_t HEIGHT = 600;

const std::vector<char const*> validationLayers = {
  "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif // NDEBUG

class Application
{
public:
	void run();

private:
	GLFWwindow                       *window          = nullptr;
  vk::raii::Context                context;
  vk::raii::Instance               instance         = nullptr;
  vk::raii::DebugUtilsMessengerEXT debugMessenger   = nullptr;
  vk::raii::SurfaceKHR             surface          = nullptr;
  vk::raii::PhysicalDevice         physicalDevice   = nullptr;
  vk::raii::Device                 device           = nullptr;
  vk::raii::Queue                  queue            = nullptr;
  uint32_t                         queueIndex       = ~0;
  vk::raii::SwapchainKHR           swapChain        = nullptr;
  std::vector<vk::Image>           swapChainImages;
  vk::SurfaceFormatKHR             swapChainSurfaceFormat;
  vk::Extent2D                     swapChainExtent;
  std::vector<vk::raii::ImageView> swapChainImageViews;

  vk::raii::PipelineLayout         pipelineLayout   = nullptr;
  vk::raii::Pipeline               graphicsPipeline = nullptr;
  vk::raii::CommandPool            commandPool      = nullptr;
  vk::raii::CommandBuffer          commandBuffer    = nullptr;

  vk::raii::Semaphore              presentCompleteSemaphore = nullptr;
  vk::raii::Semaphore              renderFinishedSemaphore  = nullptr;
  vk::raii::Fence                  drawFence                = nullptr;

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

	void initVulkan();
  void createInstance();
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createSwapChain();
  void createImageViews();
  void createGraphicsPipeline();
  void createCommandPool();
  void createCommandBuffer();

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
};
