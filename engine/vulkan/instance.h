#pragma once

#include "vulkan/core.h"

namespace vk
{
class InstanceExtension;
class Surface;

class Instance final : public WrappedObject<Instance, VkInstance>
{
  public:
    Instance() = default;

    void destroy();

    VkResult create(const VkInstanceCreateInfo& createInfo);

    Result init(Surface* surface);

  private:
    std::map<ExtensionName, InstanceExtension*> instanceExtensions;
};
} // namespace vk