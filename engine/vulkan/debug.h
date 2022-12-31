#pragma once

#include "vulkan/wrapper.h"
#include <string>

namespace vk
{
class Context;

class DebugUtilsMessenger final : public WrappedObject<DebugUtilsMessenger, VkDebugUtilsMessengerEXT>
{
  public:
    Result init(Context* context, VkDebugReportFlagsEXT flags, VkDebugReportCallbackEXT callBack);

    void terminate(VkInstance instance);

  private:
    VkResult create(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT& createInfo);
};

namespace debug
{

VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                           VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                           const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                           void* pUserData);

std::string getVkResultString(VkResult result);

} // namespace debug
} // namespace vk