#include "shadowMap.h"
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
Result ShadowMap::init(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();
    shaderParameters = new rhi::ShaderParameters();
    shaderParameters->vertexShader =
        context->allocateVertexShader("shadowmap/shadowmap.vert.spv", rhi::VertexChannel::Position);
    shaderParameters->pixelShader = context->allocatePixelShader("shadowmap/shadowmap.frag.spv");
    return Result::Continue;
}

void ShadowMap::terminate(platform::Context* context)
{
    delete shaderParameters;
    shaderParameters = nullptr;
}

Result ShadowMap::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    rhi::RenderPassInfo renderpassInfo;
    renderpassInfo.name = "Shadow map pass";
    rhi::AttachmentId depthAttachmentId = renderpassInfo.registerDepthStencilAttachment(
        {sceneInfo->getRenderTargets()->getShadowDepth()->getImage(), rhi::AttachmentLoadOp::Clear,
         rhi::AttachmentStoreOp::Store, 1, rhi::ImageLayout::DepthStencilAttachment,
         rhi::ImageLayout::DepthStencilAttachment});

    auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
    subpass.depthAttachmentReference = {depthAttachmentId, rhi::ImageLayout::DepthStencilAttachment};

    try(context->beginRenderpass(renderpassInfo));

    rhi::GraphicsPipelineState graphicsPipelineState;
    graphicsPipelineState.colorBlendState.attachmentCount = 0;
    graphicsPipelineState.assemblyState.primitiveTopology = rhi::PrimitiveTopology::TRIANGLE_LIST;
    graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
    graphicsPipelineState.rasterizationState.polygonMode = rhi::PolygonMode::FILL;
    graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::BACK_BIT;
    graphicsPipelineState.rasterizationState.depthBiasEnable = true;
    graphicsPipelineState.depthStencilState.depthTestEnable = true;
    graphicsPipelineState.depthStencilState.depthCompareOp = rhi::CompareOp::LESS_OR_EQUAL;
    graphicsPipelineState.depthStencilState.depthWriteEnable = true;
    graphicsPipelineState.dynamicState =
        rhi::DynamicState::Viewport | rhi::DynamicState::Scissor | rhi::DynamicState::DepthBias;

    graphicsPipelineState.shaderParameters = shaderParameters;
    shaderParameters->globalDescriptor = sceneInfo->getDescriptorSet();

    for (auto& model : sceneInfo->getModels())
    {
        for (auto& instance : model->getInstances())
        {
            shaderParameters->localDescriptor = instance->getDescriptorSet();
            context->createGfxPipeline(graphicsPipelineState);

            sceneInfo->getDescriptorSet()->bind(context, 0);
            instance->getDescriptorSet()->bind(context, 1);

            context->setDepthBias(depthBiasFactor, 0.0f, depthBiasSlope);
            model->draw(context);
            instance->draw(context);
        }
    }

    try(context->endRenderpass());

    return Result::Continue;
}

Result ShadowMap::updateUI()
{
    ImGui::SliderFloat("depthBiasFactor", &depthBiasFactor, 0, 10);
    ImGui::SliderFloat("depthBiasSlope", &depthBiasSlope, 0, 10);
    return Result::Continue;
}
} // namespace renderer
