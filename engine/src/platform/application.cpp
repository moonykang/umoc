#include "application.h"
#include "context.h"

namespace platform
{
void Application::init(Context* context)
{
}

void Application::loop(Context* context)
{
    ASSERT(context);

    while (context->loop() == Result::Continue)
    {
    }
}

void Application::terminate(Context* context)
{
}
} // namespace platform
