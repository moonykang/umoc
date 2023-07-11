#include "rendertargets.h"

#include "rhi/context.h"
#include "rhi/image.h"
#include "rhi/resources.h"
#include <cmath>

namespace scene
{
RenderTargets::RenderTargets()
    : sceneColor(nullptr), sceneDepth(nullptr), brdfLutTexture(nullptr), environmentCube(nullptr),
      irradianceCube(nullptr), preFilterCube(nullptr)
{
}

Result RenderTargets::init(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    rhi::Extent3D extent = context->getSurfaceSize();

    sceneColor = new rhi::Texture("SceneColor");
    try(sceneColor->init(context, rhi::Format::R8G8B8A8_UNORM, extent, 1, 1, rhi::ImageUsage::COLOR_ATTACHMENT));

    sceneDepth = new rhi::Texture("SceneDepth");
    try(sceneDepth->init(context, rhi::Format::D32_FLOAT_S8X24_UINT, extent, 1, 1,
                         rhi::ImageUsage::DEPTH_STENCIL_ATTACHMENT));

    /*
        rhi::Extent3D brdfExtent = {512, 512, 1};
        brdfLutTexture = new rhi::Texture("BRDF LUT Texture");
        try(brdfLutTexture->init(context, rhi::Format::R16G16_FLOAT, brdfExtent, 1, 1,
       rhi::ImageUsage::COLOR_ATTACHMENT));

        environmentCube = new rhi::Texture("Environment Cube Texture");
        try(environmentCube->init(context, "gcanyon_cube.ktx", platform::ImageLoader::KTX));


    // Irradiance
    {
        uint32_t dim = 64;
        uint32_t layers = 6;
        uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(dim))) + 1;
        rhi::Extent3D extent = {dim, dim, 1};

        irradianceCube = new rhi::Texture("Irradiance Cube Texture");
        try(irradianceCube->init(context, rhi::Format::R32G32B32A32_FLOAT, extent, mipLevels, layers,
                                 rhi::ImageUsage::TRANSFER_DST));
    }

    // PreFilter
    {
        uint32_t dim = 512;
        uint32_t layers = 6;
        uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(dim))) + 1;
        rhi::Extent3D extent = {dim, dim, 1};

        preFilterCube = new rhi::Texture("PreFilter Cube Texture");
        try(preFilterCube->init(context, rhi::Format::R16G16B16A16_FLOAT, extent, mipLevels, layers,
                                rhi::ImageUsage::TRANSFER_DST));
    }
        */
    return Result::Continue;
}

void RenderTargets::terminate(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    TERMINATE(sceneColor, context);
    TERMINATE(sceneDepth, context);
    TERMINATE(brdfLutTexture, context);
    TERMINATE(environmentCube, context);
    TERMINATE(irradianceCube, context);
    TERMINATE(preFilterCube, context);
}
} // namespace scene
