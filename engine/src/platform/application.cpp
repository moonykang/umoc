#include "application.h"
#include "context.h"
#include "renderer/base.h"

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

    while (context->loop() == Result::Continue)
    {
        baseRenderer->render(context);
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
