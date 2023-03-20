#include "forward.h"
#include "model/instance.h"
#include "model/material.h"
#include "model/object.h"
#include "model/vertexInput.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/defines.h"
#include "rhi/descriptor.h"
#include "rhi/image.h"
#include "rhi/rendertarget.h"
#include "rhi/resources.h"
#include "rhi/shader.h"
#include "scene/rendertargets.h"
#include "scene/scene.h"
#include "scene/testScene.h"
#include "scene/view.h"

class ForwardVertexShader : public rhi::VertexShaderBase
{
  public:
    ForwardVertexShader()
        : rhi::VertexShaderBase("forward.vert.spv",
                                rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal)
    {
    }
};

class ForwardFragmentShader : public rhi::PixelShaderBase
{
  public:
    ForwardFragmentShader() : rhi::PixelShaderBase("forward.frag.spv")
    {
    }
};

class ForwardShaderParameters : public rhi::ShaderParameters
{
  public:
    ForwardShaderParameters()
        : ShaderParameters(), globalDescriptor(nullptr), localDescriptor(nullptr), materialDescriptor(nullptr)
    {
    }

    std::vector<rhi::DescriptorSet*> getDescriptorSets() override
    {
        return {globalDescriptor, localDescriptor, materialDescriptor};
    }
    rhi::DescriptorSet* globalDescriptor;
    rhi::DescriptorSet* localDescriptor;
    rhi::DescriptorSet* materialDescriptor;
};

ForwardVertexShader forwardVertexShader;
ForwardFragmentShader forwardPixelShader;

namespace renderer
{
Result Forward::init(platform::Context* platformContext)
{
    LOGD("Init Forward pass");

    rhi::Context* context = platformContext->getRHI();

    forwardVertexShader.init(context);
    forwardPixelShader.init(context);

    return Result::Continue;
}

void Forward::terminate(platform::Context* context)
{
}

Result Forward::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();
    scene::TestScene* testScene = reinterpret_cast<scene::TestScene*>(sceneInfo);

    rhi::RenderPassInfo renderpassInfo;
    rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
        {context->getCurrentSurfaceImage(), rhi::AttachmentLoadOp::Clear, rhi::AttachmentStoreOp::Store, 1,
         rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

    rhi::AttachmentId depthAttachmentId = renderpassInfo.registerDepthStencilAttachment(
        {sceneInfo->getRenderTargets()->getSceneDepth()->getImage(), rhi::AttachmentLoadOp::Clear,
         rhi::AttachmentStoreOp::Store, 1, rhi::ImageLayout::DepthStencilAttachment,
         rhi::ImageLayout::DepthStencilAttachment});

    auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
    subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});
    subpass.depthAttachmentReference = {depthAttachmentId, rhi::ImageLayout::DepthStencilAttachment};

    try(context->addTransition(context->getCurrentSurfaceImage(), rhi::ImageLayout::ColorAttachment));
    try(context->addTransition(sceneInfo->getRenderTargets()->getSceneDepth()->getImage(),
                               rhi::ImageLayout::DepthStencilAttachment));

    try(context->beginRenderpass(renderpassInfo));

    ForwardShaderParameters params;
    params.vertexShader = &forwardVertexShader;
    params.pixelShader = &forwardPixelShader;
    params.globalDescriptor = sceneInfo->getView()->getDescriptorSet();

    rhi::GraphicsPipelineState graphicsPipelineState;
    graphicsPipelineState.shaderParameters = &params;
    graphicsPipelineState.colorBlendState.attachmentCount = 1;
    graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
    graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::BACK_BIT;
    graphicsPipelineState.depthStencilState.depthTestEnable = true;
    graphicsPipelineState.depthStencilState.depthWriteEnable = true;

    for (auto& model : testScene->getModels())
    {
        for (auto& instance : model->getInstances())
        {
            auto materialDescriptor = instance->getMaterial()->getDescriptorSet();

            params.materialDescriptor = materialDescriptor;
            params.localDescriptor = instance->getDescriptorSet();
            context->createGfxPipeline(graphicsPipelineState);

            sceneInfo->getView()->getDescriptorSet()->bind(context, 0);
            instance->getDescriptorSet()->bind(context, 1);
            materialDescriptor->bind(context, 2);

            model->draw(context);
            instance->draw(context);
        }
    }

    try(context->endRenderpass());

    return Result::Continue;
}
} // namespace renderer
