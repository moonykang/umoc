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

    DELETE(window);
}

rhi::Context* Context::getRHI()
{
    return reinterpret_cast<rhi::Context*>(this);
}
} // namespace platform