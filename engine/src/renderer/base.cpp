#include "base.h"
#include "platform/context.h"
#include "rhi/context.h"
#include "rhi/rendertarget.h"
#include "scene/scene.h"
#include "screenPass.h"

namespace renderer
{
Result BaseRenderPass::init(platform::Context* context)
{
    passes.push_back(new ScreenPass());

    for (auto& pass : passes)
    {
        // delete pass;
    }

    return Result::Continue;
}

void BaseRenderPass::terminate(platform::Context* context)
{
    for (auto& pass : passes)
    {
        delete pass;
    }
    passes.clear();
}

Result BaseRenderPass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    for (auto& pass : passes)
    {
        try(pass->render(platformContext, sceneInfo));
    }

    try(context->present());

    return Result::Continue;
}
} // namespace renderer