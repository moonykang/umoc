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
    try(sceneColor->init(context, rhi::Format::R8G8B8A8_UNORM, extent, 1, 1,
                         rhi::ImageUsage::COLOR_ATTACHMENT | rhi::ImageUsage::SAMPLED));

    sceneDepth = new rhi::Texture("SceneDepth");
    try(sceneDepth->init(context, rhi::Format::D32_FLOAT_S8X24_UINT, extent, 1, 1,
                         rhi::ImageUsage::DEPTH_STENCIL_ATTACHMENT));

    shadowDepth = new rhi::Texture("ShadowDepth");
    try(shadowDepth->init(context, rhi::Format::D32_FLOAT_S8X24_UINT, rhi::Extent3D(2048, 2048, 1), 1, 1,
                          rhi::ImageUsage::DEPTH_STENCIL_ATTACHMENT));

    computeTarget = new rhi::Texture("computeTarget");
    try(computeTarget->init(context, rhi::Format::R8G8B8A8_UNORM, extent, 1, 1,
                            rhi::ImageUsage::SAMPLED | rhi::ImageUsage::STORAGE));

    // G buffers
    gBufferA = new rhi::Texture("gBufferA");
    try(gBufferA->init(context, rhi::Format::R16G16B16A16_FLOAT, extent, 1, 1,
                       rhi::ImageUsage::COLOR_ATTACHMENT | rhi::ImageUsage::SAMPLED));
    gBufferB = new rhi::Texture("gBufferB");
    try(gBufferB->init(context, rhi::Format::R16G16B16A16_FLOAT, extent, 1, 1,
                       rhi::ImageUsage::COLOR_ATTACHMENT | rhi::ImageUsage::SAMPLED));
    gBufferC = new rhi::Texture("gBufferC");
    try(gBufferC->init(context, rhi::Format::R16G16B16A16_FLOAT, extent, 1, 1,
                       rhi::ImageUsage::COLOR_ATTACHMENT | rhi::ImageUsage::SAMPLED));
    gBufferD = new rhi::Texture("gBufferD");
    try(gBufferD->init(context, rhi::Format::R16G16B16A16_FLOAT, extent, 1, 1,
                       rhi::ImageUsage::COLOR_ATTACHMENT | rhi::ImageUsage::SAMPLED));

    // SSAO
    ssao = new rhi::Texture("SSAO");
    try(ssao->init(context, rhi::Format::R8_UNORM, extent, 1, 1,
                   rhi::ImageUsage::COLOR_ATTACHMENT | rhi::ImageUsage::SAMPLED));
    ssaoBlur = new rhi::Texture("ssaoBlur");
    try(ssaoBlur->init(context, rhi::Format::R16G16B16A16_FLOAT, extent, 1, 1,
                       rhi::ImageUsage::COLOR_ATTACHMENT | rhi::ImageUsage::SAMPLED));

    // Bloomp
    bloomSetup = new rhi::Texture("BloomSetup");
    try(bloomSetup->init(context, rhi::Format::R8G8B8A8_UNORM, extent, 1, 1,
                         rhi::ImageUsage::COLOR_ATTACHMENT | rhi::ImageUsage::SAMPLED));

    bloomHorizontal = new rhi::Texture("bloomHorizontal");
    try(bloomHorizontal->init(context, rhi::Format::R8G8B8A8_UNORM, extent, 1, 1,
                              rhi::ImageUsage::COLOR_ATTACHMENT | rhi::ImageUsage::SAMPLED));

    bloomVertical = new rhi::Texture("bloomVertical");
    try(bloomVertical->init(context, rhi::Format::R8G8B8A8_UNORM, extent, 1, 1,
                            rhi::ImageUsage::COLOR_ATTACHMENT | rhi::ImageUsage::SAMPLED));

    uint32_t white = -1;
    whiteDummy = new rhi::Texture("whiteDummy");
    try(whiteDummy->init(context, rhi::Format::R8G8B8A8_UNORM, {1, 1, 1},
                         rhi::ImageUsage::COLOR_ATTACHMENT | rhi::ImageUsage::SAMPLED, sizeof(uint32_t), &white));

    uint32_t black = 0;
    blackDummy = new rhi::Texture("blackDummy");
    try(blackDummy->init(context, rhi::Format::R8G8B8A8_UNORM, {1, 1, 1},
                         rhi::ImageUsage::COLOR_ATTACHMENT | rhi::ImageUsage::SAMPLED, sizeof(uint32_t), &black));

    // BRDF
    {
        rhi::Extent3D brdfExtent = {512, 512, 1};
        brdfLutTexture = new rhi::Texture("BRDF LUT Texture");
        try(brdfLutTexture->init(context, rhi::Format::R16G16_FLOAT, brdfExtent, 1, 1,
                                 rhi::ImageUsage::COLOR_ATTACHMENT | rhi::ImageUsage::SAMPLED));

        environmentCube = new rhi::Texture("Environment Cube Texture");
        try(environmentCube->init(context, "uffizi_cube.ktx", platform::ImageLoader::KTX));
    }

    // Irradiance
    {
        uint32_t dim = 64;
        uint32_t layers = 6;
        uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(dim))) + 1;
        rhi::Extent3D extent = {dim, dim, 1};

        irradianceCube = new rhi::Texture("Irradiance Cube Texture");
        try(irradianceCube->init(context, rhi::Format::R32G32B32A32_FLOAT, extent, mipLevels, layers,
                                 rhi::ImageUsage::TRANSFER_DST | rhi::ImageUsage::SAMPLED));
    }

    // PreFilter
    {
        uint32_t dim = 512;
        uint32_t layers = 6;
        uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(dim))) + 1;
        rhi::Extent3D extent = {dim, dim, 1};

        preFilterCube = new rhi::Texture("PreFilter Cube Texture");
        try(preFilterCube->init(context, rhi::Format::R16G16B16A16_FLOAT, extent, mipLevels, layers,
                                rhi::ImageUsage::TRANSFER_DST | rhi::ImageUsage::SAMPLED));
    }

    return Result::Continue;
}

void RenderTargets::terminate(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    TERMINATE(sceneColor, context);
    TERMINATE(sceneDepth, context);

    TERMINATE(gBufferA, context);
    TERMINATE(gBufferB, context);
    TERMINATE(gBufferC, context);

    TERMINATE(ssao, context);

    TERMINATE(brdfLutTexture, context);
    TERMINATE(environmentCube, context);
    TERMINATE(irradianceCube, context);
    TERMINATE(preFilterCube, context);

    TERMINATE(bloomSetup, context);
    TERMINATE(bloomHorizontal, context);
    TERMINATE(bloomVertical, context);

    TERMINATE(whiteDummy, context);
    TERMINATE(blackDummy, context);
}
} // namespace scene
