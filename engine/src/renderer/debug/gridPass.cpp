
#include "gridPass.h"
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

namespace renderer
{
Result GridPass::init(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    // setup
    {
        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "debug/grid.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal);
        shaderParameters.pixelShader = context->allocatePixelShader("debug/grid.frag.spv");

        auto loader = model::predefined::Loader::Builder().setShaderParameters(&shaderParameters).build();

        object = loader->load(platformContext, sceneInfo);
        instance = object->instantiate(platformContext, glm::mat4(1.0f), true);
    }

    return Result::Continue;
}

void GridPass::terminate(platform::Context* context)
{
    TERMINATE(object, context);
}

Result GridPass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    // setup
    {
        rhi::RenderPassInfo renderpassInfo;
        renderpassInfo.name = "Grid Pass (Debug)";

        rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
            {sceneInfo->getRenderTargets()->getSceneColor()->getImage(), rhi::AttachmentLoadOp::Load,
             rhi::AttachmentStoreOp::Store, 1, rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

        rhi::AttachmentId depthAttachmentId = renderpassInfo.registerDepthStencilAttachment(
            {sceneInfo->getRenderTargets()->getSceneDepth()->getImage(), rhi::AttachmentLoadOp::Load,
             rhi::AttachmentStoreOp::Store, 1, rhi::ImageLayout::DepthStencilAttachment,
             rhi::ImageLayout::DepthStencilAttachment});

        auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
        subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});
        subpass.depthAttachmentReference = {depthAttachmentId, rhi::ImageLayout::DepthStencilAttachment};

        try(context->addTransition(sceneInfo->getRenderTargets()->getSceneColor()->getImage(),
                                   rhi::ImageLayout::ColorAttachment));

        try(context->beginRenderpass(renderpassInfo));

        auto material = instance->getMaterial();

        rhi::ShaderParameters* shaderParameters = material->getShaderParameters();
        shaderParameters->globalDescriptor = sceneInfo->getDescriptorSet();

        rhi::GraphicsPipelineState graphicsPipelineState;
        graphicsPipelineState.shaderParameters = shaderParameters;
        graphicsPipelineState.colorBlendState.attachmentCount = 1;
        graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
        graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::FRONT_BIT;
        graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].srcColorBlendFactor =
            rhi::BlendFactor::SRC_ALPHA;
        graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].dstColorBlendFactor =
            rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
        graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].srcAlphaBlendFactor =
            rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
        graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].dstAlphaBlendFactor =
            rhi::BlendFactor::ZERO;
        graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].colorBlendOp = rhi::BlendOp::ADD;
        graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].alphaBlendOp = rhi::BlendOp::ADD;
        graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].colorWriteMask =
            rhi::ColorComponent::R | rhi::ColorComponent::G | rhi::ColorComponent::B | rhi::ColorComponent::A;
        graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].blendEnable = true;
        graphicsPipelineState.depthStencilState.depthTestEnable = true;
        graphicsPipelineState.depthStencilState.depthCompareOp = rhi::CompareOp::LESS_OR_EQUAL;
        graphicsPipelineState.depthStencilState.depthWriteEnable = true;

        context->createGfxPipeline(graphicsPipelineState);

        sceneInfo->getDescriptorSet()->bind(context, 0);

        object->draw(context);
        instance->draw(context);

        try(context->endRenderpass());
    }

    return Result::Continue;
}
} // namespace renderer