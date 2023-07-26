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
#include "environment/brdfLutPass.h"
#include "environment/irradiancePass.h"
#include "environment/preFilterPass.h"
#include "postprocess/bloom.h"
#include "postprocess/screen.h"
#include "sceneRenderer/deferred.h"
#include "sceneRenderer/forward.h"
#include "sceneRenderer/lighting.h"
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
        if (renderingOptions.useForwardRendering())
        {
            passes.push_back(new SSAOPass());
        }
        else
        {
            passes.push_back(new Lighting());
        }
    }

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
    // passes.push_back(new UIPass());

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
    try(sceneInfo->udpate(platformContext));

    for (auto& pass : passes)
    {
        try(pass->render(platformContext, sceneInfo));
    }

    /*
        auto surfaceImage = context->getCurrentSurfaceImage();
        static bool bCapture = true;

        // Do one time
        if (bCapture)
        {
            rhi::ImageSubResource imageSubResource = {};
            imageSubResource.layerCount = 1;
            imageSubResource.levelCount = 1;

            size_t surfaceImageSize = surfaceImage->getSize(imageSubResource);
            char* captureData = new char[surfaceImageSize];

            try(context->readBackImage(surfaceImage, imageSubResource, captureData));

            LOGD("Success to capture");
            rhi::Extent3D extent = surfaceImage->getExtent();
            std::ofstream fout;
            fout.open(std::string(PPM_PATH) + "/temp.ppm");

            if (fout.is_open())
            {
                fout << "P3\n" << extent.width << ' ' << extent.height << "\n255\n";

                size_t numPixels = surfaceImageSize / 4;

                size_t pixel = 0;
                for (int i = 0; i < numPixels; i++)
                {
                    fout << captureData[pixel++] << ' ' << captureData[pixel++] << ' ' << captureData[pixel++];
                    pixel++;
                }
                fout.close();
            }
            delete[] captureData;
            bCapture = false;
        }
    */
    try(context->present());

    return Result::Continue;
}
} // namespace renderer
