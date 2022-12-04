#pragma once

#include "vulkan/wrapper.h"
#include <string>

namespace vk
{
namespace debug
{

VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                           VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                           const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                           void* pUserData);

class DebugCallback
{
  public:
    Result init(VkInstance instance, VkDebugReportFlagsEXT flags, VkDebugReportCallbackEXT callBack);

    void terminate(VkInstance instance);

  private:
    handle::DebugUtilsMessenger debugUtilsMessenger;
};

std::string getVkResultString(VkResult result);

} // namespace debug
} // namespace vk