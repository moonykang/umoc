#include "lighting.h"
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
Result Lighting::init(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    LOGD("Init Lighting pass");

    rhi::Context* context = platformContext->getRHI();

    // Lighting
    {
        model::Material* material = new model::Material();
        try(material->init(platformContext));
        material->updateTexture(model::MaterialFlag::External, sceneInfo->getRenderTargets()->getGBufferA(),
                                rhi::ShaderStage::Pixel);
        material->updateTexture(model::MaterialFlag::External, sceneInfo->getRenderTargets()->getGBufferB(),
                                rhi::ShaderStage::Pixel);
        material->updateTexture(model::MaterialFlag::External, sceneInfo->getRenderTargets()->getGBufferC(),
                                rhi::ShaderStage::Pixel);
        material->updateTexture(model::MaterialFlag::External, sceneInfo->getRenderTargets()->getSceneDepth(),
                                rhi::ShaderStage::Pixel);
        material->updateTexture(model::MaterialFlag::External, sceneInfo->getRenderTargets()->getShadowDepth(),
                                rhi::ShaderStage::Pixel);
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

void Lighting::terminate(platform::Context* context)
{
    TERMINATE(lightingObject, context);
}

Result Lighting::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

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
        try(context->addTransition(sceneInfo->getRenderTargets()->getSceneDepth()->getImage(),
                                   rhi::ImageLayout::DepthStencilAttachmentReadOnly));
        try(context->addTransition(sceneInfo->getRenderTargets()->getShadowDepth()->getImage(),
                                   rhi::ImageLayout::DepthStencilAttachmentReadOnly));
        try(context->beginRenderpass(renderpassInfo));

        auto material = lightingInstance->getMaterial();
        auto materialDescriptor = material->getDescriptorSet();

        rhi::ShaderParameters* shaderParameters = material->getShaderParameters();
        shaderParameters->globalDescriptor = sceneInfo->getDescriptorSet();
        shaderParameters->materialDescriptor = materialDescriptor;

        rhi::GraphicsPipelineState graphicsPipelineState;
        graphicsPipelineState.shaderParameters = shaderParameters;
        graphicsPipelineState.colorBlendState.attachmentCount = 1;
        graphicsPipelineState.rasterizationState.polygonState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
        graphicsPipelineState.rasterizationState.polygonState.cullMode = rhi::CullMode::FRONT_BIT;

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
