#include "rendertargets.h"

#include "rhi/context.h"
#include "rhi/image.h"
#include "rhi/resources.h"

namespace scene
{
RenderTargets::RenderTargets() : sceneColor(nullptr), sceneDepth(nullptr)
{
}

Result RenderTargets::init(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    rhi::Extent3D extent = context->getSurfaceSize();

    sceneColor = new rhi::Texture();
    try(sceneColor->init(context, rhi::Format::R8G8B8A8_UNORM, extent, rhi::ImageUsage::COLOR_ATTACHMENT));

    sceneDepth = new rhi::Texture();
    try(sceneDepth->init(context, rhi::Format::D32_FLOAT_S8X24_UINT, extent,
                         rhi::ImageUsage::DEPTH_STENCIL_ATTACHMENT));

    return Result::Continue;
}

void RenderTargets::terminate(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    TERMINATE(sceneColor, context);
    TERMINATE(sceneDepth, context);
}

rhi::Texture* RenderTargets::getSceneColor()
{
    return sceneColor;
}

rhi::Texture* RenderTargets::getSceneDepth()
{
    return sceneDepth;
}
} // namespace scene