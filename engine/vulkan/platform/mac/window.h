#pragma once

#include "platform/mac/window.h"
#include "vulkan/platform/window.h"
#include "vulkan/surface.h"

namespace vk
{
namespace mac
{
class Window : public platform::mac::Window, public vk::Window
{
  public:
    void initRHI() override;

    void initSurface(VkInstance instance, Surface* surface) override final;
};
} // namespace mac
} // namespace vk