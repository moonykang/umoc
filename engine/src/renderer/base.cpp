#include "base.h"
#include "platform/context.h"
#include "rhi/context.h"
#include "rhi/image.h"
#include "rhi/rendertarget.h"
#include "scene/rendertargets.h"
#include "scene/scene.h"
#include "ui.h"

#include "compute/filter.h"
#include "compute/nbody.h"
#include "compute/particle.h"
#include "debug/gridLayerPass.h"
#include "environment/brdfLutPass.h"
#include "environment/irradiancePass.h"
#include "environment/preFilterPass.h"
#include "postprocess/bloom.h"
#include "postprocess/screen.h"
#include "sceneRenderer/deferred.h"
#include "sceneRenderer/forward.h"
#include "sceneRenderer/lighting.h"
#include "sceneRenderer/shadowMap.h"
#include "sceneRenderer/ssao.h"

#include <fstream>

namespace renderer
{
Result BaseRenderPass::init(platform::Context* context, scene::SceneInfo* sceneInfo)
{
    auto renderingOptions = sceneInfo->getRenderingOptions();

    if (renderingOptions.useEnvironmap())
    {
        passes.push_back(new BrdfLutPass());
        passes.push_back(new IrradiancePass());
        passes.push_back(new PreFilterPass());
    }

    if (renderingOptions.useParticleRendering())
    {
        passes.push_back(new NBodyPass());
    }
    else
    {
        passes.push_back(new ShadowMap());

        if (renderingOptions.useForwardRendering())
        {
            passes.push_back(new Forward());
        }
        else
        {
            passes.push_back(new Deferred());

            if (renderingOptions.useForwardRendering())
            {
                passes.push_back(new SSAOPass());
            }
            else
            {
                passes.push_back(new Lighting());
            }
        }
    }

    passes.push_back(new GridLayerPass());

    if (renderingOptions.getComputePostProcess() != ComputePostProcess::None)
    {
        passes.push_back(new compute::FilterPass());
        renderingOptions.setFinalTarget(sceneInfo->getRenderTargets()->getComputeTarget());
    }

    if (renderingOptions.useBloom())
    {
        passes.push_back(new BloomPass());
    }
    else
    {
        passes.push_back(new ScreenPass(renderingOptions.getFinalTarget()));
    }

    passes.push_back(new UIPass());

    for (auto& pass : passes)
    {
        try(pass->init(context, sceneInfo));
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
    try(sceneInfo->update(platformContext));

    for (auto& pass : passes)
    {
        try(pass->updateUI());
    }
    try(sceneInfo->postUpdate(platformContext));

    for (auto& pass : passes)
    {
        try(pass->render(platformContext, sceneInfo));
    }
    try(context->present());

    return Result::Continue;
}
} // namespace renderer
