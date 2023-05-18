#include "base.h"
#include "bloom.h"
#include "brdfLutPass.h"
#include "forward.h"
#include "irradiancePass.h"
#include "platform/context.h"
#include "preFilterPass.h"
#include "rhi/context.h"
#include "rhi/rendertarget.h"
#include "scene/scene.h"
#include "ui.h"

namespace renderer
{
Result BaseRenderPass::init(platform::Context* context, scene::SceneInfo* sceneInfo)
{
    // passes.push_back(new BrdfLutPass());
    // passes.push_back(new IrradiancePass());
    // passes.push_back(new PreFilterPass());
    passes.push_back(new Forward());
    passes.push_back(new BloomPass());
    // passes.push_back(new UIPass());

    for (auto& pass : passes)
    {
        try(pass->init(context, sceneInfo));
        // delete pass;
    }

    return Result::Continue;
}

void BaseRenderPass::terminate(platform::Context* context)
{
    for (auto pass : passes)
    {
        pass->terminate(context);
        delete pass;
    }
    passes.clear();
}

Result BaseRenderPass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    // scene update
    try(sceneInfo->udpate(platformContext));

    for (auto& pass : passes)
    {
        try(pass->render(platformContext, sceneInfo));
    }

    try(context->present());

    return Result::Continue;
}
} // namespace renderer
