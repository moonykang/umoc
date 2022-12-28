#pragma once

#include "vulkan/core.h"
#include "vulkan/swapchain.h"

namespace platform
{
class Window;
}

namespace vk
{
class PhysicalDevice;

class Surface : public WrappedObject<Surface, VkSurfaceKHR>
{
  public:
    static Surface* createPlatformSurface();

    Surface();

    virtual ~Surface() = default;

  public:
    virtual Result init(platform::Window* window, VkInstance instance) = 0;

    void terminate(VkInstance instance);

    virtual std::vector<std::string> getSurfaceExtensions() = 0;

    Result updateSurfaceCapabilities(PhysicalDevice* physicalDevice);

  private:
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<VkPresentModeKHR> presentModes;
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
};
} // namespace vk
