#pragma once

#include "common/util.h"
#include "core.h"
#include "vulkan/extension.h"
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

class DebugMarker
{
};

} // namespace vk