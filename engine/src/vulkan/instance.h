#pragma once

#include "vulkan/core.h"

namespace vk
{
class InstanceExtension;
class Context;

class Instance final : public WrappedObject<Instance, VkInstance>
{
  public:
    Instance() = default;

    Result init(Context* context);

    void terminate();

    bool supportExtension(ExtensionName extensionName);

  private:
    VkResult create(const VkInstanceCreateInfo& createInfo);

  private:
    std::map<ExtensionName, InstanceExtension*> instanceExtensions;
};
} // namespace vk