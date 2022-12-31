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

    Result init(Surface* surface);

    void terminate();

  private:
    VkResult create(const VkInstanceCreateInfo& createInfo);

  private:
    std::map<ExtensionName, InstanceExtension*> instanceExtensions;
};
} // namespace vk