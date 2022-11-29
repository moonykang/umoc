#include "platform/application.h"
#include "platform/context.h"
#include "rhi/context.h"
#include <cstdlib>

int main()
{
    platform::Context* context = rhi::Context::createRHIContext(rhi::List::Vulkan);
    context->init();

    platform::Application application;
    application.init(context);
    application.loop(context);
    application.terminate(context);

    context->terminate();

    delete context;
    return EXIT_SUCCESS;
}