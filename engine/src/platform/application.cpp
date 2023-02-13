#include "application.h"
#include "context.h"
#include "renderer/base.h"
#include <exception>

namespace platform
{
Application::Application()
{
}

Result Application::init(Context* context)
{
    return Result::Continue;
}

void Application::loop(Context* context)
{
    ASSERT(context);

    size_t debug_loop_count = 0;
    try
    {
        while (context->loop() == Result::Continue && debug_loop_count++ < 120)
        {
            renderer::BaseRenderPass baseRenderpass;
            baseRenderpass.render(context);
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
}
} // namespace platform
