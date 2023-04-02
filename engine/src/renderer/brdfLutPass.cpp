#include "brdfLutPass.h"
#include "model/instance.h"
#include "model/material.h"
#include "model/object.h"
#include "model/predefined/loader.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"
#include "rhi/image.h"
#include "rhi/shader.h"
#include "scene/rendertargets.h"
#include "scene/scene.h"

class BrdfLutVertexShader : public rhi::VertexShaderBase
{
  public:
    BrdfLutVertexShader()
        : rhi::VertexShaderBase("screen.vert.spv",
                                rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal)
    {
    }
};

class BrdfLutFragmentShader : public rhi::PixelShaderBase
{
  public:
    BrdfLutFragmentShader() : rhi::PixelShaderBase("brdflut.frag.spv")
    {
    }
};

class BrdfLutShaderParameters : public rhi::ShaderParameters
{
  public:
    BrdfLutShaderParameters() : ShaderParameters()
    {
    }

    std::vector<rhi::DescriptorSet*> getDescriptorSets() override
    {
        return {};
    }
};

BrdfLutVertexShader brdfLutVertexShader;
BrdfLutFragmentShader brdfLutPixelShader;

namespace renderer
{
Result BrdfLutPass::init(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    auto loader = model::predefined::Loader::Builder().build();

    object = loader->load(platformContext, sceneInfo);
    instance = object->instantiate(platformContext, glm::mat4(1.0f), true);

    brdfLutVertexShader.init(context);
    brdfLutPixelShader.init(context);

    return Result::Continue;
}

void BrdfLutPass::terminate(platform::Context* context)
{
    TERMINATE(object, context);
}

Result BrdfLutPass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    rhi::RenderPassInfo renderpassInfo;
    renderpassInfo.name = "BRDF LUT Pass";

    rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
        {sceneInfo->getRenderTargets()->getBrdfLutTexture()->getImage(), rhi::AttachmentLoadOp::Clear,
         rhi::AttachmentStoreOp::Store, 1, rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

    auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
    subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});

    try(context->addTransition(sceneInfo->getRenderTargets()->getBrdfLutTexture()->getImage(),
                               rhi::ImageLayout::ColorAttachment));

    try(context->beginRenderpass(renderpassInfo));

    auto materialDescriptor = instance->getMaterial()->getDescriptorSet();

    BrdfLutShaderParameters params;
    params.vertexShader = &brdfLutVertexShader;
    params.pixelShader = &brdfLutPixelShader;

    rhi::GraphicsPipelineState graphicsPipelineState;
    graphicsPipelineState.shaderParameters = &params;
    graphicsPipelineState.colorBlendState.attachmentCount = 1;
    graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
    graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::FRONT_BIT;
    context->createGfxPipeline(graphicsPipelineState);

    object->draw(context);
    instance->draw(context);

    try(context->endRenderpass());

    return Result::Continue;
}
} // namespace renderer
