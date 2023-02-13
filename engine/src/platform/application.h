#pragma once

#include "common/util.h"

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
};
} // namespace platform