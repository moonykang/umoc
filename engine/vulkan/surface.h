#pragma once

#include "vulkan/core.h"

namespace platform
{
class Window;
}

namespace vk
{
class Surface
{
  public:
    static Surface* createPlatformSurface();

    Surface();

    virtual ~Surface() = default;

  public:
    virtual Result initSurface(platform::Window* window, VkInstance instance) = 0;

    void terminate(VkInstance instance);

    virtual std::vector<std::string> getSurfaceExtensions() = 0;

  public:
    VkSurfaceKHR& getSurface();

  protected:
    VkSurfaceKHR surface;
};
} // namespace vk
