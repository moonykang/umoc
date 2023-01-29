#include "context.h"
#include "rhi/context.h"

namespace platform
{
Context::Context() : asset(nullptr), window(nullptr)
{
}

Result Context::init()
{
    window = Window::createPlatformWindow();

    window->init();

    try(initRHI(window));

    return Result::Continue;
}

void Context::terminate()
{
    terminateRHI();

    TERMINATE(window);
}

Result Context::loop()
{
    ASSERT(window);

    return window->valid() ? Result::Continue : Result::Fail;
}

rhi::Context* Context::getRHI()
{
    return reinterpret_cast<rhi::Context*>(this);
}
} // namespace platform