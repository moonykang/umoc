#include "vulkan/surface.h"

namespace vk
{
VkSurfaceKHR& Surface::getSurface()
{
    return surface;
}
} // namespace vk