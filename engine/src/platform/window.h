#pragma once

#include "common/util.h"

/*
platform::Window
> platform::<platform>Window
> rhi::<platform>window
*/
namespace platform
{
class Input;
class Window
{
  public:
    virtual ~Window() = default;

    virtual void init() = 0;

    virtual void terminate() = 0;

    virtual bool valid() = 0;

    virtual Input* getInput() = 0;

  public:
    static Window* createPlatformWindow();
};
} // namespace platform