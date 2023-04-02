#include "preFilterPass.h"
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

class PreFilterPassVertexShader : public rhi::VertexShaderBase
{
  public:
    PreFilterPassVertexShader()
        : rhi::VertexShaderBase("filtercube.vert.spv",
                                rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal)
    {
    }
};

class PreFilterPassFragmentShader : public rhi::PixelShaderBase
{
  public:
    PreFilterPassFragmentShader() : rhi::PixelShaderBase("prefilterenvmap.frag.spv")
    {
    }
};

class PreFilterPassShaderParameters : public rhi::ShaderParameters
{
  public:
    PreFilterPassShaderParameters() : ShaderParameters(), materialDescriptor(nullptr)
    {
    }

    std::vector<rhi::DescriptorSet*> getDescriptorSets() override
    {
        return {materialDescriptor};
    }

    rhi::DescriptorSet* materialDescriptor;
};

namespace preFilterPass
{
struct PushBlock
{
    glm::mat4 mvp;
    float roughness;
    uint32_t numSamples = 32u;
} pushBlock;
} // namespace preFilterPass

PreFilterPassVertexShader preFilterPassVertexShader;
PreFilterPassFragmentShader preFilterPassPixelShader;

namespace renderer
{
class PreFilterPassMaterial : public model::Material
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

Result PreFilterPass::init(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    PreFilterPassMaterial* material = new PreFilterPassMaterial();
    material->updateTexture(model::MaterialFlag::BaseColorTexture, sceneInfo->getRenderTargets()->getEnvironmentCube());

    auto loader = model::gltf::Loader::Builder()
                      .setFileName("cube.gltf")
                      .setGltfLoadingFlags(model::GltfLoadingFlag::FlipY)
                      .addExternalMaterial(material)
                      .build();

    object = loader->load(platformContext, sceneInfo);
    instance = object->instantiate(platformContext, glm::mat4(1.0f), true);

    try(preFilterPassVertexShader.init(context));
    try(preFilterPassPixelShader.init(context));

    return Result::Continue;
}

void PreFilterPass::terminate(platform::Context* context)
{
    TERMINATE(object, context);
}

Result PreFilterPass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    if (oneTimeRender)
    {
        return Result::Continue;
    }

    rhi::Context* context = platformContext->getRHI();

    rhi::Texture* offscreenTexture = new rhi::Texture();
    rhi::Texture* preFilterTexture = sceneInfo->getRenderTargets()->getPreFilterCube();

    const int32_t dim = 512;
    const uint32_t numMips = static_cast<uint32_t>(std::floor(std::log2(dim))) + 1;

    try(offscreenTexture->init(context, "Offscreen Texture (PrefilterPass)", rhi::Format::R16G16B16A16_FLOAT,
                               {dim, dim, 1}, 1, 1, rhi::ImageUsage::COLOR_ATTACHMENT | rhi::ImageUsage::TRANSFER_SRC));

    rhi::RenderPassInfo renderpassInfo;
    renderpassInfo.name = "PreFilter Pass";

    rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
        {offscreenTexture->getImage(), rhi::AttachmentLoadOp::Clear, rhi::AttachmentStoreOp::Store, 1,
         rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

    auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
    subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});

    auto materialDescriptor = instance->getMaterial()->getDescriptorSet();

    PreFilterPassShaderParameters params;
    params.vertexShader = &preFilterPassVertexShader;
    params.pixelShader = &preFilterPassPixelShader;
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
        rhi::PushConstant(rhi::ShaderStage::Vertex | rhi::ShaderStage::Pixel, 0, sizeof(preFilterPass::PushBlock)));

    for (uint32_t level = 0; level < numMips; level++)
    {
        preFilterPass::pushBlock.roughness = (float)level / (float)(numMips - 1);
        for (uint32_t face = 0; face < 6; face++)
        {
            rhi::Extent2D extent;
            extent.width = static_cast<uint32_t>(dim * std::pow(0.5f, level));
            extent.height = static_cast<uint32_t>(dim * std::pow(0.5f, level));

            try(context->addTransition(offscreenTexture->getImage(), rhi::ImageLayout::ColorAttachment));

            try(context->beginRenderpass(renderpassInfo));
            try(context->viewport(extent));
            try(context->createGfxPipeline(graphicsPipelineState));

            preFilterPass::pushBlock.mvp = glm::perspective((float)(M_PI / 2.0), 1.0f, 0.1f, 512.0f) * matrices[face];
            context->pushConstant(rhi::ShaderStage::Vertex | rhi::ShaderStage::Pixel, sizeof(preFilterPass::PushBlock),
                                  &preFilterPass::pushBlock);

            materialDescriptor->bind(context, 0);

            object->draw(context);
            instance->draw(context);

            try(context->endRenderpass());

            rhi::ImageSubResource srcImageSubResource = {};

            rhi::ImageSubResource dstImageSubResource = {};
            dstImageSubResource.baseArrayLayer = 0;
            dstImageSubResource.baseMipLevel = 0;

            try(context->copyImage(offscreenTexture->getImage(), srcImageSubResource, preFilterTexture->getImage(),
                                   dstImageSubResource, {64, 64, 1}));
        }
    }

    TERMINATE(offscreenTexture, context);

    oneTimeRender = true;
    return Result::Continue;
}
} // namespace renderer
