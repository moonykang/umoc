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
    asset = Asset::createPlatformAsset();

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
    timer.tick();
    return window->valid() ? Result::Continue : Result::Fail;
}

rhi::Context* Context::getRHI()
{
    return reinterpret_cast<rhi::Context*>(this);
}

Asset* Context::getAssetManager()
{
    ASSERT(asset);
    return asset;
}

Window* Context::getWindow()
{
    return window;
}

Timer& Context::getTimer()
{
    return timer;
}
} // namespace platform