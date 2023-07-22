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
namespace bloom
{
struct PushBlock
{
    int direction;
} pushBlock;
} // namespace bloom

class BloomSetupMaterial : public model::Material
{
  public:
    struct MaterialUniformBlock
    {
        float brightThreshold;

        MaterialUniformBlock() : brightThreshold(1.f)
        {
        }
    } ubo;

    Result init(platform::Context* platformContext) override
    {
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
        uniformBuffer = {context->allocateUniformBuffer(sizeof(MaterialUniformBlock), &ubo), rhi::ShaderStage::Pixel};

        return model::Material::init(platformContext);
    }

    Result update(platform::Context* platformContext) override
    {
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
        try(uniformBuffer.first->update(context, sizeof(MaterialUniformBlock), &ubo));

        return model::Material::update(platformContext);
    }
};

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
        uniformBuffer = {context->allocateUniformBuffer(sizeof(MaterialUniformBlock), &ubo), rhi::ShaderStage::Pixel};

        return model::Material::init(platformContext);
    }

    Result update(platform::Context* platformContext) override
    {
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
        try(uniformBuffer.first->update(context, sizeof(MaterialUniformBlock), &ubo));

        return model::Material::update(platformContext);
    }
};

Result BloomPass::init(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    // setup
    {
        BloomSetupMaterial* material = new BloomSetupMaterial();
        try(material->init(platformContext));
        material->updateTexture(model::MaterialFlag::BaseColorTexture, sceneInfo->getRenderTargets()->getSceneColor(),
                                rhi::ShaderStage::Pixel);
        try(material->update(platformContext));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "screen/screen.vert.spv",
            rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal);
        shaderParameters.pixelShader = context->allocatePixelShader("bloom/filter.frag.spv");

        auto loader =
            model::predefined::Loader::Builder().setMaterial(material).setShaderParameters(&shaderParameters).build();

        bloomSetupObject = loader->load(platformContext, sceneInfo);
        bloomSetupInstance = bloomSetupObject->instantiate(platformContext, glm::mat4(1.0f), true);
    }

    // blur horizontal
    {
        BloomMaterial* bloomMaterial = new BloomMaterial();
        try(bloomMaterial->init(platformContext));
        bloomMaterial->updateTexture(model::MaterialFlag::BaseColorTexture,
                                     sceneInfo->getRenderTargets()->getBloomSetup(), rhi::ShaderStage::Pixel);
        try(bloomMaterial->update(platformContext));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "screen/screen.vert.spv",
            rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal);
        shaderParameters.pixelShader = context->allocatePixelShader("bloom/blur.frag.spv");

        auto loader = model::predefined::Loader::Builder()
                          .setMaterial(bloomMaterial)
                          .setShaderParameters(&shaderParameters)
                          .build();

        bloomHorizontalObject = loader->load(platformContext, sceneInfo);
        bloomHorizontalInstance = bloomHorizontalObject->instantiate(platformContext, glm::mat4(1.0f), true);
    }

    // blur vertical
    {
        BloomMaterial* bloomMaterial = new BloomMaterial();
        try(bloomMaterial->init(platformContext));
        bloomMaterial->updateTexture(model::MaterialFlag::BaseColorTexture,
                                     sceneInfo->getRenderTargets()->getBloomHorizontal(), rhi::ShaderStage::Pixel);
        try(bloomMaterial->update(platformContext));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "screen/screen.vert.spv",
            rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal);
        shaderParameters.pixelShader = context->allocatePixelShader("bloom/blur.frag.spv");

        auto loader = model::predefined::Loader::Builder()
                          .setMaterial(bloomMaterial)
                          .setShaderParameters(&shaderParameters)
                          .build();

        bloomVerticalObject = loader->load(platformContext, sceneInfo);
        bloomVerticalInstance = bloomVerticalObject->instantiate(platformContext, glm::mat4(1.0f), true);
    }

    // bloom composite
    {
        BloomMaterial* bloomMaterial = new BloomMaterial();
        try(bloomMaterial->init(platformContext));
        bloomMaterial->updateTexture(model::MaterialFlag::BaseColorTexture,
                                     sceneInfo->getRenderTargets()->getSceneColor(), rhi::ShaderStage::Pixel);
        bloomMaterial->updateTexture(model::MaterialFlag::External, sceneInfo->getRenderTargets()->getBloomVertical(),
                                     rhi::ShaderStage::Pixel);
        try(bloomMaterial->update(platformContext));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "screen/screen.vert.spv",
            rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal);
        shaderParameters.pixelShader = context->allocatePixelShader("bloom/composite.frag.spv");

        auto loader = model::predefined::Loader::Builder()
                          .setMaterial(bloomMaterial)
                          .setShaderParameters(&shaderParameters)
                          .build();

        bloomCompositeObject = loader->load(platformContext, sceneInfo);
        bloomCompositeInstance = bloomCompositeObject->instantiate(platformContext, glm::mat4(1.0f), true);
    }

    return Result::Continue;
}

void BloomPass::terminate(platform::Context* context)
{
    TERMINATE(bloomSetupObject, context);
    TERMINATE(bloomHorizontalObject, context);
    TERMINATE(bloomVerticalObject, context);
    TERMINATE(bloomCompositeObject, context);
}

Result BloomPass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    // setup
    {
        rhi::RenderPassInfo renderpassInfo;
        renderpassInfo.name = "Bloom Setup Pass";

        rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
            {sceneInfo->getRenderTargets()->getBloomSetup()->getImage(), rhi::AttachmentLoadOp::Clear,
             rhi::AttachmentStoreOp::Store, 1, rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

        auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
        subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});

        try(context->addTransition(sceneInfo->getRenderTargets()->getBloomSetup()->getImage(),
                                   rhi::ImageLayout::ColorAttachment));
        try(context->addTransition(sceneInfo->getRenderTargets()->getSceneColor()->getImage(),
                                   rhi::ImageLayout::FragmentShaderReadOnly));

        try(context->beginRenderpass(renderpassInfo));

        auto material = bloomSetupInstance->getMaterial();
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

        bloomSetupObject->draw(context);
        bloomSetupInstance->draw(context);

        try(context->endRenderpass());
    }

    // Bloom horizontal
    {
        rhi::RenderPassInfo renderpassInfo;
        renderpassInfo.name = "Bloom Blur Horizontal Pass";

        rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
            {sceneInfo->getRenderTargets()->getBloomHorizontal()->getImage(), rhi::AttachmentLoadOp::Clear,
             rhi::AttachmentStoreOp::Store, 1, rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

        auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
        subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});

        try(context->addTransition(sceneInfo->getRenderTargets()->getBloomHorizontal()->getImage(),
                                   rhi::ImageLayout::ColorAttachment));
        try(context->addTransition(sceneInfo->getRenderTargets()->getBloomSetup()->getImage(),
                                   rhi::ImageLayout::FragmentShaderReadOnly));

        try(context->beginRenderpass(renderpassInfo));

        auto material = bloomHorizontalInstance->getMaterial();
        auto materialDescriptor = material->getDescriptorSet();

        rhi::ShaderParameters* shaderParameters = material->getShaderParameters();
        shaderParameters->materialDescriptor = materialDescriptor;

        rhi::GraphicsPipelineState graphicsPipelineState;
        graphicsPipelineState.shaderParameters = shaderParameters;
        graphicsPipelineState.colorBlendState.attachmentCount = 1;
        graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
        graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::FRONT_BIT;
        graphicsPipelineState.pushConstants.push_back(
            rhi::PushConstant(rhi::ShaderStage::Pixel, 0, sizeof(bloom::PushBlock)));

        context->createGfxPipeline(graphicsPipelineState);

        bloom::pushBlock.direction = 0;
        context->pushConstant(rhi::ShaderStage::Pixel, sizeof(bloom::PushBlock), &bloom::pushBlock);

        materialDescriptor->bind(context, 0);

        bloomHorizontalObject->draw(context);
        bloomHorizontalInstance->draw(context);

        try(context->endRenderpass());
    }

    // Bloom vertical
    {
        rhi::RenderPassInfo renderpassInfo;
        renderpassInfo.name = "Bloom Blur Vertical Pass";

        rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
            {sceneInfo->getRenderTargets()->getBloomVertical()->getImage(), rhi::AttachmentLoadOp::Clear,
             rhi::AttachmentStoreOp::Store, 1, rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

        auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
        subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});

        try(context->addTransition(sceneInfo->getRenderTargets()->getBloomVertical()->getImage(),
                                   rhi::ImageLayout::ColorAttachment));
        try(context->addTransition(sceneInfo->getRenderTargets()->getBloomHorizontal()->getImage(),
                                   rhi::ImageLayout::FragmentShaderReadOnly));

        try(context->beginRenderpass(renderpassInfo));

        auto material = bloomVerticalInstance->getMaterial();
        auto materialDescriptor = material->getDescriptorSet();

        rhi::ShaderParameters* shaderParameters = material->getShaderParameters();
        shaderParameters->materialDescriptor = materialDescriptor;

        rhi::GraphicsPipelineState graphicsPipelineState;
        graphicsPipelineState.shaderParameters = shaderParameters;
        graphicsPipelineState.colorBlendState.attachmentCount = 1;
        graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
        graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::FRONT_BIT;
        graphicsPipelineState.pushConstants.push_back(
            rhi::PushConstant(rhi::ShaderStage::Pixel, 0, sizeof(bloom::PushBlock)));

        context->createGfxPipeline(graphicsPipelineState);

        bloom::pushBlock.direction = 0;
        context->pushConstant(rhi::ShaderStage::Pixel, sizeof(bloom::PushBlock), &bloom::pushBlock);

        materialDescriptor->bind(context, 0);

        bloomVerticalObject->draw(context);
        bloomVerticalInstance->draw(context);

        try(context->endRenderpass());
    }

    // Bloom composite
    {
        rhi::RenderPassInfo renderpassInfo;
        renderpassInfo.name = "Bloom Composite Pass";

        rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
            {context->getCurrentSurfaceImage(), rhi::AttachmentLoadOp::Clear, rhi::AttachmentStoreOp::Store, 1,
             rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

        auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
        subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});

        try(context->addTransition(context->getCurrentSurfaceImage(), rhi::ImageLayout::ColorAttachment));
        try(context->addTransition(sceneInfo->getRenderTargets()->getBloomVertical()->getImage(),
                                   rhi::ImageLayout::FragmentShaderReadOnly));
        try(context->addTransition(sceneInfo->getRenderTargets()->getSceneColor()->getImage(),
                                   rhi::ImageLayout::FragmentShaderReadOnly));
        try(context->beginRenderpass(renderpassInfo));

        auto material = bloomCompositeInstance->getMaterial();
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

        bloomCompositeObject->draw(context);
        bloomCompositeInstance->draw(context);

        try(context->endRenderpass());
    }

    return Result::Continue;
}
} // namespace renderer