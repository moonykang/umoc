#pragma once

namespace platform
{
class Context;
class Application
{
  public:
    void init(Context* context);
    void loop(Context* context);
    void terminate(Context* context);
};
} // namespace platform