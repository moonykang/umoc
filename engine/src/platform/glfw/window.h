#pragma once

#include "platform/window.h"

namespace platform
{
namespace glfw
{
class Window : public platform::Window
{
  public:
    Window();

    void init() override;

    void terminate() override;

    bool valid() override;

    platform::Input* getInput() override;

    GLFWwindow* getWindow();

  private:
    GLFWwindow* window;
};
} // namespace glfw
} // namespace platform