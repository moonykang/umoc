#include "application.h"
#include "context.h"
#include "renderer/base.h"
#include "scene/scene.h"
#include "scene/testScene.h"
#include <exception>

namespace platform
{
Application::Application() : sceneInfo(nullptr)
{
}

Result Application::init(Context* context)
{
    sceneInfo = new scene::TestScene();
    sceneInfo->init(context);
    sceneInfo->load(context);

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
            baseRenderpass.render(context, sceneInfo);
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
}
} // namespace platform
