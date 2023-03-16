#include "application.h"
#include "context.h"
#include "renderer/base.h"
#include "scene/scene.h"
#include "scene/testScene.h"
#include <exception>

namespace platform
{
Application::Application() : sceneInfo(nullptr), baseRenderpass(nullptr)
{
}

Result Application::init(Context* context)
{
    sceneInfo = new scene::TestScene();
    try(sceneInfo->init(context));
    try(sceneInfo->load(context));

    baseRenderpass = new renderer::BaseRenderPass();
    try(baseRenderpass->init(context));

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
            try_call(baseRenderpass->render(context, sceneInfo));
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
    TERMINATE(sceneInfo, context);
    TERMINATE(baseRenderpass, context);
}
} // namespace platform
