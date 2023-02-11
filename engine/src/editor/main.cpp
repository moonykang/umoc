#include "common/util.h"
#include "platform/application.h"
#include "platform/context.h"
#include "rhi/context.h"
#include <cstdlib>

int main()
{
    LOGD("Begin");
    platform::Context* context = rhi::Context::createRHIContext(rhi::List::Vulkan);

    if (context->init() == Result::Fail)
    {
        LOGE("Failed to init context");
        return EXIT_FAILURE;
    }

    platform::Application application;
    application.init(context);
    application.loop(context);
    application.terminate(context);

    context->terminate();

    delete context;

    LOGD("Terminate");
    return EXIT_SUCCESS;
}