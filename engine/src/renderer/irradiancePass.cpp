#include "irradiancePass.h"
#include "model/gltf/loader.h"
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
#include <cmath>

class IrradianceVertexShader : public rhi::VertexShaderBase
{
  public:
    IrradianceVertexShader()
        : rhi::VertexShaderBase("filtercube.vert.spv",
                                rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal)
    {
    }
};

class IrradianceFragmentShader : public rhi::PixelShaderBase
{
  public:
    IrradianceFragmentShader() : rhi::PixelShaderBase("irradiancecube.frag.spv")
    {
    }
};

class IrradianceShaderParameters : public rhi::ShaderParameters
{
  public:
    IrradianceShaderParameters() : ShaderParameters(), materialDescriptor(nullptr)
    {
    }

    std::vector<rhi::DescriptorSet*> getDescriptorSets() override
    {
        return {materialDescriptor};
    }

    rhi::DescriptorSet* materialDescriptor;
};

struct PushBlock
{
    glm::mat4 mvp;
    // Sampling deltas
    float deltaPhi = (2.0f * float(M_PI)) / 180.0f;
    float deltaTheta = (0.5f * float(M_PI)) / 64.0f;
} pushBlock;

IrradianceVertexShader irradianceVertexShader;
IrradianceFragmentShader irradiancePixelShader;

namespace renderer
{
class IrradianceMaterial : public model::Material
{
  public:
    Result init(platform::Context* platformContext) override
    {
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

        return model::Material::init(platformContext);
    }

    Result update(platform::Context* platformContext) override
    {
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

        return model::Material::update(platformContext);
    }
};

Result IrradiancePass::init(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    IrradianceMaterial* material = new IrradianceMaterial();
    material->updateTexture(model::MaterialFlag::BaseColorTexture, sceneInfo->getRenderTargets()->getEnvironmentCube());

    try(material->init(context));
    try(material->update(context));

    auto loader = model::gltf::Loader::Builder()
                      .setFileName("cube.gltf")
                      .setGltfLoadingFlags(model::GltfLoadingFlag::FlipY)
                      .addExternalMaterial(material)
                      .build();

    object = loader->load(platformContext, sceneInfo);
    instance = object->instantiate(platformContext, glm::mat4(1.0f), true);

    try(irradianceVertexShader.init(context));
    try(irradiancePixelShader.init(context));

    return Result::Continue;
}

void IrradiancePass::terminate(platform::Context* context)
{
    TERMINATE(object, context);
}

Result IrradiancePass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    if (oneTimeRender)
    {
        return Result::Continue;
    }

    rhi::Context* context = platformContext->getRHI();

    rhi::Texture* offscreenTexture = new rhi::Texture();
    rhi::Texture* irradianceTexture = sceneInfo->getRenderTargets()->getIrradianceCube();

    const int32_t dim = 64;
    const uint32_t numMips = static_cast<uint32_t>(std::floor(std::log2(dim))) + 1;
    try(offscreenTexture->init(context, "Offscreen Texture", rhi::Format::R32G32B32A32_FLOAT, {dim, dim, 1}, 1, 1,
                               rhi::ImageUsage::COLOR_ATTACHMENT | rhi::ImageUsage::TRANSFER_SRC));

    try(context->addTransition(sceneInfo->getRenderTargets()->getEnvironmentCube()->getImage(),
                               rhi::ImageLayout::FragmentShaderReadOnly));

    rhi::RenderPassInfo renderpassInfo;
    renderpassInfo.name = "Irradiance Pass";

    rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
        {offscreenTexture->getImage(), rhi::AttachmentLoadOp::Clear, rhi::AttachmentStoreOp::Store, 1,
         rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

    auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
    subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});

    auto materialDescriptor = instance->getMaterial()->getDescriptorSet();

    IrradianceShaderParameters params;
    params.vertexShader = &irradianceVertexShader;
    params.pixelShader = &irradiancePixelShader;
    params.materialDescriptor = materialDescriptor;

    std::vector<glm::mat4> matrices = {
        // POSITIVE_X
        glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
                    glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        // NEGATIVE_X
        glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
                    glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        // POSITIVE_Y
        glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        // NEGATIVE_Y
        glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        // POSITIVE_Z
        glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        // NEGATIVE_Z
        glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    };

    rhi::GraphicsPipelineState graphicsPipelineState;
    graphicsPipelineState.shaderParameters = &params;
    graphicsPipelineState.colorBlendState.attachmentCount = 1;
    graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
    graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::NONE;
    graphicsPipelineState.pushConstants.push_back(
        rhi::PushConstant(rhi::ShaderStage::Vertex | rhi::ShaderStage::Pixel, 0, sizeof(PushBlock)));

    for (uint32_t level = 0; level < numMips; level++)
    {
        for (uint32_t face = 0; face < 6; face++)
        {
            rhi::Extent2D extent;
            extent.width = static_cast<uint32_t>(dim * std::pow(0.5f, level));
            extent.height = static_cast<uint32_t>(dim * std::pow(0.5f, level));

            try(context->addTransition(offscreenTexture->getImage(), rhi::ImageLayout::ColorAttachment));

            try(context->beginRenderpass(renderpassInfo));
            try(context->viewport(extent));
            try(context->createGfxPipeline(graphicsPipelineState));

            pushBlock.mvp = glm::perspective((float)(M_PI / 2.0), 1.0f, 0.1f, 512.0f) * matrices[face];
            context->pushConstant(rhi::ShaderStage::Vertex | rhi::ShaderStage::Pixel, sizeof(PushBlock), &pushBlock);

            materialDescriptor->bind(context, 0);

            object->draw(context);
            instance->draw(context);

            try(context->endRenderpass());

            rhi::ImageSubResource srcImageSubResource = {};

            rhi::ImageSubResource dstImageSubResource = {};
            dstImageSubResource.baseArrayLayer = face;
            dstImageSubResource.baseMipLevel = level;

            try(context->copyImage(offscreenTexture->getImage(), srcImageSubResource, irradianceTexture->getImage(),
                                   dstImageSubResource, {extent.width, extent.height, 1}));
        }
    }

    TERMINATE(offscreenTexture, context);

    oneTimeRender = true;
    return Result::Continue;
}
} // namespace renderer
