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
    LOGD("Init Deferred pass");

    rhi::Context* context = platformContext->getRHI();

    // Lighting
    {
        model::Material* material = new model::Material();
        try(material->init(platformContext));
        material->updateTexture(model::MaterialFlag::External, sceneInfo->getRenderTargets()->getGBufferA());
        material->updateTexture(model::MaterialFlag::External, sceneInfo->getRenderTargets()->getGBufferB());
        material->updateTexture(model::MaterialFlag::External, sceneInfo->getRenderTargets()->getGBufferC());
        try(material->update(platformContext));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "screen/screen.vert.spv",
            rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal);
        shaderParameters.pixelShader = context->allocatePixelShader("deferred/lighting.frag.spv");

        auto loader =
            model::predefined::Loader::Builder().setMaterial(material).setShaderParameters(&shaderParameters).build();

        lightingObject = loader->load(platformContext, sceneInfo);
        lightingInstance = lightingObject->instantiate(platformContext, glm::mat4(1.0f), true);
    }

    return Result::Continue;
}

void Deferred::terminate(platform::Context* context)
{
    TERMINATE(lightingObject, context);
}

Result Deferred::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    // Deferred Geometry pass
    {
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
        graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::NONE;
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
    }

    // Deferred lighting pass
    {
        rhi::RenderPassInfo renderpassInfo;
        renderpassInfo.name = "Deferred lighting pass";

        rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
            {sceneInfo->getRenderTargets()->getSceneColor()->getImage(), rhi::AttachmentLoadOp::Clear,
             rhi::AttachmentStoreOp::Store, 1, rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

        auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
        subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});

        try(context->addTransition(sceneInfo->getRenderTargets()->getSceneColor()->getImage(),
                                   rhi::ImageLayout::ColorAttachment));
        try(context->addTransition(sceneInfo->getRenderTargets()->getGBufferA()->getImage(),
                                   rhi::ImageLayout::FragmentShaderReadOnly));
        try(context->addTransition(sceneInfo->getRenderTargets()->getGBufferB()->getImage(),
                                   rhi::ImageLayout::FragmentShaderReadOnly));
        try(context->addTransition(sceneInfo->getRenderTargets()->getGBufferC()->getImage(),
                                   rhi::ImageLayout::FragmentShaderReadOnly));
        try(context->beginRenderpass(renderpassInfo));

        auto material = lightingInstance->getMaterial();
        auto materialDescriptor = material->getDescriptorSet();

        rhi::ShaderParameters* shaderParameters = material->getShaderParameters();
        shaderParameters->globalDescriptor = sceneInfo->getDescriptorSet();
        shaderParameters->materialDescriptor = materialDescriptor;

        rhi::GraphicsPipelineState graphicsPipelineState;
        graphicsPipelineState.shaderParameters = shaderParameters;
        graphicsPipelineState.colorBlendState.attachmentCount = 1;
        graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
        graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::NONE;

        context->createGfxPipeline(graphicsPipelineState);

        sceneInfo->getDescriptorSet()->bind(context, 0);
        materialDescriptor->bind(context, 1);

        lightingObject->draw(context);
        lightingInstance->draw(context);

        try(context->endRenderpass());
    }

    return Result::Continue;
}
} // namespace renderer
