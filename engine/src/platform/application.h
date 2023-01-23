#pragma once

#include "common/util.h"

namespace renderer
{
class Base;
}

namespace platform
{
class Context;
class Application
{
  public:
    Application();

    ~Application() = default;

    Result init(Context* context);

    void loop(Context* context);

    void terminate(Context* context);

  private:
    renderer::Base* baseRenderer;
};
} // namespace platform