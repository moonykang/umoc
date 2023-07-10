#include "bloom.h"
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

class BloomMaterial : public model::Material
{
  public:
    struct MaterialUniformBlock
    {
        float blurScale;
        float blurStrength;

        MaterialUniformBlock() : blurScale(1.f), blurStrength(1.5f)
        {
        }
    } ubo;

    Result init(platform::Context* platformContext) override
    {
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
        uniformBuffer = context->allocateUniformBuffer(sizeof(MaterialUniformBlock), &ubo);

        return model::Material::init(platformContext);
    }

    Result update(platform::Context* platformContext) override
    {
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
        try(uniformBuffer->update(context, sizeof(MaterialUniformBlock), &ubo));

        return model::Material::update(platformContext);
    }
};

Result BloomPass::init(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    BloomMaterial* bloomMaterial = new BloomMaterial();
    try(bloomMaterial->init(platformContext));
    bloomMaterial->updateTexture(model::MaterialFlag::BaseColorTexture, sceneInfo->getRenderTargets()->getSceneColor());
    try(bloomMaterial->update(platformContext));

    rhi::ShaderParameters shaderParameters;
    shaderParameters.vertexShader = context->allocateVertexShader(
        "screen/screen.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal);
    shaderParameters.pixelShader = context->allocatePixelShader("screen/screen.frag.spv");

    auto loader =
        model::predefined::Loader::Builder().setMaterial(bloomMaterial).setShaderParameters(&shaderParameters).build();

    object = loader->load(platformContext, sceneInfo);
    instance = object->instantiate(platformContext, glm::mat4(1.0f), true);

    return Result::Continue;
}

void BloomPass::terminate(platform::Context* context)
{
    TERMINATE(object, context);
}

Result BloomPass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    rhi::RenderPassInfo renderpassInfo;
    renderpassInfo.name = "Bloom Pass";

    rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
        {context->getCurrentSurfaceImage(), rhi::AttachmentLoadOp::Clear, rhi::AttachmentStoreOp::Store, 1,
         rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

    auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
    subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});

    try(context->addTransition(context->getCurrentSurfaceImage(), rhi::ImageLayout::ColorAttachment));
    try(context->addTransition(sceneInfo->getRenderTargets()->getSceneColor()->getImage(),
                               rhi::ImageLayout::FragmentShaderReadOnly));

    try(context->beginRenderpass(renderpassInfo));

    auto material = instance->getMaterial();
    auto materialDescriptor = material->getDescriptorSet();

    rhi::ShaderParameters* shaderParameters = material->getShaderParameters();
    shaderParameters->materialDescriptor = materialDescriptor;

    rhi::GraphicsPipelineState graphicsPipelineState;
    graphicsPipelineState.shaderParameters = shaderParameters;
    graphicsPipelineState.colorBlendState.attachmentCount = 1;
    graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
    graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::FRONT_BIT;
    context->createGfxPipeline(graphicsPipelineState);

    materialDescriptor->bind(context, 0);

    object->draw(context);
    instance->draw(context);

    try(context->endRenderpass());

    return Result::Continue;
}
} // namespace renderer