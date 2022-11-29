#pragma once

#include "platform/window.h"

namespace platform
{
namespace mac
{
class Window : public platform::Window
{
  public:
    Window();

    void init() override;

    void terminate() override;

    void initGlfw();

    virtual void initRHI() = 0;

  protected:
    GLFWwindow* window;
};
} // namespace mac
} // namespace platform