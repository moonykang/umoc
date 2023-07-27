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
#include "scene/view.h"

namespace renderer
{

Result Forward::init(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    LOGD("Init Forward pass");
    return Result::Continue;
}

void Forward::terminate(platform::Context* context)
{
}

Result Forward::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    rhi::RenderPassInfo renderpassInfo;
    renderpassInfo.name = "Forward Pass";

    rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
        {sceneInfo->getRenderTargets()->getSceneColor()->getImage(), rhi::AttachmentLoadOp::Clear,
         rhi::AttachmentStoreOp::Store, 1, rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

    rhi::AttachmentId depthAttachmentId = renderpassInfo.registerDepthStencilAttachment(
        {sceneInfo->getRenderTargets()->getSceneDepth()->getImage(), rhi::AttachmentLoadOp::Clear,
         rhi::AttachmentStoreOp::Store, 1, rhi::ImageLayout::DepthStencilAttachment,
         rhi::ImageLayout::DepthStencilAttachment});

    auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
    subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});
    subpass.depthAttachmentReference = {depthAttachmentId, rhi::ImageLayout::DepthStencilAttachment};

    try(context->beginRenderpass(renderpassInfo));

<<<<<<< HEAD
    rhi::GraphicsPipelineState graphicsPipelineState;
    graphicsPipelineState.colorBlendState.attachmentCount = 1;
    graphicsPipelineState.assemblyState.primitiveTopology = rhi::PrimitiveTopology::TRIANGLE_LIST;
    graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
    graphicsPipelineState.rasterizationState.polygonMode = rhi::PolygonMode::FILL;
    graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::BACK_BIT;
    graphicsPipelineState.depthStencilState.depthTestEnable = true;
    graphicsPipelineState.depthStencilState.depthCompareOp = rhi::CompareOp::LESS_OR_EQUAL;
    graphicsPipelineState.depthStencilState.depthWriteEnable = true;

=======
>>>>>>> d3143e2 (Update IBL)
    int idx = 0;
    for (auto& model : sceneInfo->getModels())
    {
        for (auto& instance : model->getInstances())
        {

            rhi::GraphicsPipelineState graphicsPipelineState;
            graphicsPipelineState.colorBlendState.attachmentCount = 1;
            graphicsPipelineState.assemblyState.primitiveTopology = rhi::PrimitiveTopology::TRIANGLE_LIST;
            graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
            graphicsPipelineState.rasterizationState.polygonMode = rhi::PolygonMode::FILL;
            graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::FRONT_BIT;
            graphicsPipelineState.depthStencilState.depthTestEnable = true;
            graphicsPipelineState.depthStencilState.depthCompareOp = rhi::CompareOp::LESS_OR_EQUAL;
            graphicsPipelineState.depthStencilState.depthWriteEnable = true;

            auto& constantBlock = instance->getPushConstantBlock();

            if (constantBlock.valid())
            {
                graphicsPipelineState.pushConstants.push_back(
                    rhi::PushConstant(constantBlock.getShaderStage(), 0, constantBlock.getSize()));
            }

            model::Material* material = instance->getMaterial();
            auto materialDescriptor = material->getDescriptorSet();

            rhi::ShaderParameters* shaderParameters = material->getShaderParameters();
            shaderParameters->globalDescriptor = sceneInfo->getDescriptorSet();
            shaderParameters->materialDescriptor = materialDescriptor;
            shaderParameters->localDescriptor = instance->getDescriptorSet();

            graphicsPipelineState.shaderParameters = shaderParameters;

            context->createGfxPipeline(graphicsPipelineState);

            if (constantBlock.valid())
            {
                constantBlock.push(context);
            }

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
