#include "application.h"
#include "context.h"
#include "renderer/base.h"
#include <exception>

namespace platform
{
Application::Application() : baseRenderer(nullptr)
{
}

Result Application::init(Context* context)
{
    baseRenderer = new renderer::Base();
    try(baseRenderer->init(context));

    return Result::Continue;
}

void Application::loop(Context* context)
{
    ASSERT(context);

    size_t debug_loop_count = 0;
    try
    {
        while (context->loop() == Result::Continue && debug_loop_count++ < 6)
        {
            baseRenderer->render(context);
        }
    }
    catch (std::exception& e)
    {
        LOGE("Exception %s", e.what());
        return;
    }
}

void Application::terminate(Context* context)
{
    if (baseRenderer)
    {
        delete baseRenderer;
        baseRenderer = nullptr;
    }
}
} // namespace platform
