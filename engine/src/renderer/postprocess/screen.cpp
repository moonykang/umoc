#include "screen.h"
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
ScreenPass::ScreenPass(rhi::Texture* finalTarget) : finalTarget(finalTarget)
{
}

Result ScreenPass::init(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    // setup
    {
        model::Material* material = new model::Material();
        try(material->init(platformContext));

        if (finalTarget)
        {
            material->updateTexture(model::MaterialFlag::BaseColorTexture, finalTarget, rhi::ShaderStage::Pixel);
        }
        else
        {
            material->updateTexture(model::MaterialFlag::BaseColorTexture,
                                    sceneInfo->getRenderTargets()->getSceneColor(), rhi::ShaderStage::Pixel);
        }
        try(material->update(platformContext));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "screen/screen.vert.spv",
            rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal);
        shaderParameters.pixelShader = context->allocatePixelShader("screen/screen.frag.spv");

        auto loader =
            model::predefined::Loader::Builder().setMaterial(material).setShaderParameters(&shaderParameters).build();

        object = loader->load(platformContext, sceneInfo);
        instance = object->instantiate(platformContext, glm::mat4(1.0f), true);
    }

    return Result::Continue;
}

void ScreenPass::terminate(platform::Context* context)
{
    TERMINATE(object, context);
}

Result ScreenPass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    // setup
    {
        rhi::RenderPassInfo renderpassInfo;
        renderpassInfo.name = "Screen Pass";

        rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
            {context->getCurrentSurfaceImage(), rhi::AttachmentLoadOp::Clear, rhi::AttachmentStoreOp::Store, 1,
             rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

        auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
        subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});

        try(context->addTransition(context->getCurrentSurfaceImage(), rhi::ImageLayout::ColorAttachment));
        if (finalTarget)
        {
            try(context->addTransition(finalTarget->getImage(), rhi::ImageLayout::FragmentShaderReadOnly));
        }
        else
        {
            try(context->addTransition(sceneInfo->getRenderTargets()->getSceneColor()->getImage(),
                                       rhi::ImageLayout::FragmentShaderReadOnly));
        }

        try(context->beginRenderpass(renderpassInfo));

        auto material = instance->getMaterial();
        auto materialDescriptor = material->getDescriptorSet();

        rhi::ShaderParameters* shaderParameters = material->getShaderParameters();
        shaderParameters->materialDescriptor = materialDescriptor;

        rhi::GraphicsPipelineState graphicsPipelineState;
        graphicsPipelineState.shaderParameters = shaderParameters;
        graphicsPipelineState.colorBlendState.attachmentCount = 1;
        graphicsPipelineState.rasterizationState.polygonState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
        graphicsPipelineState.rasterizationState.polygonState.cullMode = rhi::CullMode::FRONT_BIT;

        context->createGfxPipeline(graphicsPipelineState);

        materialDescriptor->bind(context, 0);

        object->draw(context);
        instance->draw(context);

        try(context->endRenderpass());
    }

    return Result::Continue;
}
} // namespace renderer