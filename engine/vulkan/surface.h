#pragma once

#include "vulkan/core.h"

namespace platform
{
class Window;
}

namespace vk
{
class Context;

class Surface : public WrappedObject<Surface, VkSurfaceKHR>
{
  public:
    static Surface* createPlatformSurface();

    Surface();

    virtual ~Surface() = default;

  public:
    virtual Result init(platform::Window* window, Context* context) = 0;

    void terminate(VkInstance instance);

    virtual std::vector<std::string> getSurfaceExtensions() = 0;

    VkSurfaceCapabilitiesKHR getSurfaceCapabilities(VkPhysicalDevice physicalDevice);

    std::vector<VkPresentModeKHR> getSurfacePresentModes(VkPhysicalDevice physicalDevice);

    VkSurfaceFormatKHR getSurfaceFormat(VkPhysicalDevice physicalDevice);
};
} // namespace vk
