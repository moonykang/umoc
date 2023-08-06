#include "deferred.h"
#include "model/instance.h"
#include "model/material.h"
#include "model/object.h"
#include "model/predefined/loader.h"
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
#include "scene/view.h"

namespace renderer
{
Result Deferred::init(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    return Result::Continue;
}

void Deferred::terminate(platform::Context* context)
{
}

Result Deferred::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    rhi::RenderPassInfo renderpassInfo;
    renderpassInfo.name = "Deferred Geometry pass";

    rhi::AttachmentId gBufferAattachmentId = renderpassInfo.registerColorAttachment(
        {sceneInfo->getRenderTargets()->getGBufferA()->getImage(), rhi::AttachmentLoadOp::Clear,
         rhi::AttachmentStoreOp::Store, 1, rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});
    rhi::AttachmentId gBufferBattachmentId = renderpassInfo.registerColorAttachment(
        {sceneInfo->getRenderTargets()->getGBufferB()->getImage(), rhi::AttachmentLoadOp::Clear,
         rhi::AttachmentStoreOp::Store, 1, rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});
    rhi::AttachmentId gBufferCattachmentId = renderpassInfo.registerColorAttachment(
        {sceneInfo->getRenderTargets()->getGBufferC()->getImage(), rhi::AttachmentLoadOp::Clear,
         rhi::AttachmentStoreOp::Store, 1, rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

    rhi::AttachmentId depthAttachmentId = renderpassInfo.registerDepthStencilAttachment(
        {sceneInfo->getRenderTargets()->getSceneDepth()->getImage(), rhi::AttachmentLoadOp::Clear,
         rhi::AttachmentStoreOp::Store, 1, rhi::ImageLayout::DepthStencilAttachment,
         rhi::ImageLayout::DepthStencilAttachment});

    auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
    subpass.colorAttachmentReference.push_back({gBufferAattachmentId, rhi::ImageLayout::ColorAttachment});
    subpass.colorAttachmentReference.push_back({gBufferBattachmentId, rhi::ImageLayout::ColorAttachment});
    subpass.colorAttachmentReference.push_back({gBufferCattachmentId, rhi::ImageLayout::ColorAttachment});
    subpass.depthAttachmentReference = {depthAttachmentId, rhi::ImageLayout::DepthStencilAttachment};

    try(context->beginRenderpass(renderpassInfo));

    rhi::GraphicsPipelineState graphicsPipelineState;
    graphicsPipelineState.colorBlendState.attachmentCount = 3;
    graphicsPipelineState.assemblyState.primitiveTopology = rhi::PrimitiveTopology::TRIANGLE_LIST;
    graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
    graphicsPipelineState.rasterizationState.polygonMode = rhi::PolygonMode::FILL;
    graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::BACK_BIT;
    graphicsPipelineState.depthStencilState.depthTestEnable = true;
    graphicsPipelineState.depthStencilState.depthCompareOp = rhi::CompareOp::LESS_OR_EQUAL;
    graphicsPipelineState.depthStencilState.depthWriteEnable = true;

    for (auto& model : sceneInfo->getModels())
    {
        for (auto& instance : model->getInstances())
        {
            model::Material* material = instance->getMaterial();
            auto materialDescriptor = material->getDescriptorSet();

            rhi::ShaderParameters* shaderParameters = material->getShaderParameters();
            shaderParameters->globalDescriptor = sceneInfo->getDescriptorSet();
            shaderParameters->materialDescriptor = materialDescriptor;
            shaderParameters->localDescriptor = instance->getDescriptorSet();

            graphicsPipelineState.shaderParameters = shaderParameters;

            context->createGfxPipeline(graphicsPipelineState);

            sceneInfo->getDescriptorSet()->bind(context, 0);
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
