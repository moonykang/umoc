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

    virtual ~Window() = default;

    virtual void init() = 0;

    virtual void terminate() = 0;

    virtual bool valid() = 0;

  public:
    static Window* createPlatformWindow();

  private:
    Input* input;
};
} // namespace platform