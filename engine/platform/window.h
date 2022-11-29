#pragma once

#include "common/util.h"
#include "input.h"

/*
platform::Window
> platform::<platform>Window
> rhi::<platform>window
*/
namespace platform
{
class Window
{
  public:
    Window();
    ~Window() = default;

    virtual void init() = 0;

    virtual void terminate() = 0;

  private:
    Input* input;
};
} // namespace platform