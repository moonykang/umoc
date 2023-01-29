#pragma once

#include "vulkan/surface.h"

namespace platform
{
class Window;
}

namespace vk
{
namespace glfw
{
class Surface : public vk::Surface
{
  public:
    Result init(platform::Window* window, Context* context) override final;

    std::vector<std::string> getSurfaceExtensions() override final;
};
} // namespace mac
} // namespace vk