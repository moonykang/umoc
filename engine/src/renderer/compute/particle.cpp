#include "particle.h"
#include "model/instance.h"
#include "model/material.h"
#include "model/object.h"
#include "model/predefined/loader.h"
#include "platform/input.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"
#include "rhi/image.h"
#include "rhi/shader.h"
#include "scene/rendertargets.h"
#include "scene/scene.h"
#include "scene/textures.h"
#include <random>

#define PARTICLE_COUNT 256 * 1024

namespace renderer
{

struct Particle
{
    glm::vec2 pos;
    glm::vec2 vel;
    glm::vec4 gradientPos;
};

struct ParticlePushBlock
{
    float width;
    float height;
} pushBlock;

class ParticleMaterial : public model::Material
{
  public:
    struct MaterialUniformBlock
    {                 // Compute shader uniform block object
        float deltaT; //		Frame delta time
        float destX;  //		x position of the attractor
        float destY;  //		y position of the attractor
        int32_t particleCount = PARTICLE_COUNT;
    } ubo;

    Result init(platform::Context* platformContext) override
    {
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
        uniformBuffer = {context->allocateUniformBuffer(sizeof(MaterialUniformBlock), &ubo), rhi::ShaderStage::Compute};

        return model::Material::init(platformContext);
    }

    Result update(platform::Context* platformContext) override
    {
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
        try(uniformBuffer.first->update(context, sizeof(MaterialUniformBlock), &ubo));

        return model::Material::update(platformContext);
    }

    Result updateUniformBuffer(platform::Context* platformContext, float deltaT, float destX, float destY)
    {
        ubo.deltaT = deltaT;
        ubo.destX = destX;
        ubo.destY = destY;

        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
        return uniformBuffer.first->update(context, sizeof(MaterialUniformBlock), &ubo);
    }
};

Result ParticlePass::init(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    //
    std::default_random_engine rndEngine(0);
    std::uniform_real_distribution<float> rndDist(-1.0f, 1.0f);

    // Initial particle positions
    std::vector<Particle> particleBuffer(PARTICLE_COUNT);
    for (auto& particle : particleBuffer)
    {
        particle.pos = glm::vec2(rndDist(rndEngine), rndDist(rndEngine));
        particle.vel = glm::vec2(0.0f);
        particle.gradientPos.x = particle.pos.x / 2.0f;
    }

    size_t storageBufferSize = particleBuffer.size() * sizeof(Particle);

    auto storageBuffer = context->allocateStorageBuffer(storageBufferSize, particleBuffer.data());

    // Compute
    {
        ParticleMaterial* material = new ParticleMaterial();
        try(material->init(platformContext));
        material->updateStorageBuffer(storageBuffer, rhi::ShaderStage::Compute);
        try(material->update(platformContext));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.computeShader = context->allocateComputeShader("particle/particle.comp.spv");

        auto loader =
            model::predefined::Loader::Builder().setMaterial(material).setShaderParameters(&shaderParameters).build();

        object = loader->load(platformContext, sceneInfo);
        instance = object->instantiate(platformContext, glm::mat4(1.0f), true);
    }

    // Graphics
    {
        model::Material* material = new model::Material();
        try(material->init(platformContext));
        // texture 0
        {
            auto [id, texture] =
                sceneInfo->getTextures()->get(context, "Particle RGBA", "particles/particle01_rgba.ktx");
            material->updateTexture(model::MaterialFlag::External, texture, rhi::ShaderStage::Pixel);
        }
        // texture 1
        {
            auto [id, texture] =
                sceneInfo->getTextures()->get(context, "Particle Gradient", "particles/particle_gradient_rgba.ktx");
            material->updateTexture(model::MaterialFlag::External, texture, rhi::ShaderStage::Pixel);
        }
        try(material->update(platformContext));

        rhi::ShaderParameters shaderParameters;
        std::vector<rhi::VertexAttribute> vertexAttributes = {
            rhi::VertexAttribute(rhi::Format::R32G32_FLOAT, offsetof(Particle, pos)),
            rhi::VertexAttribute(rhi::Format::R32G32_FLOAT, offsetof(Particle, vel)),
            rhi::VertexAttribute(rhi::Format::R32G32B32A32_FLOAT, offsetof(Particle, gradientPos)),
        };

        shaderParameters.vertexShader =
            context->allocateVertexShader("particle/particle.vert.spv", vertexAttributes, sizeof(Particle));
        shaderParameters.pixelShader = context->allocatePixelShader("particle/particle.frag.spv");

        auto loader = model::predefined::Loader::Builder()
                          .setPredefineModelType(model::PredefinedModel::Storage)
                          .setMaterial(material)
                          .setShaderParameters(&shaderParameters)
                          .setExternalVertexBuffer({storageBuffer, PARTICLE_COUNT})
                          .build();

        graphicsObject = loader->load(platformContext, sceneInfo);
        graphicsInstance = graphicsObject->instantiate(platformContext, glm::mat4(1.0f), true);
    }

    return Result::Continue;
}

void ParticlePass::terminate(platform::Context* context)
{
    TERMINATE(object, context);
    TERMINATE(graphicsObject, context);
}

Result ParticlePass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    uint64_t time = platformContext->getTimer().getCurrentTime();
    float frameTimer = platformContext->getTimer().getCurrentFrameTimer();
    // compute
    {
        ParticleMaterial* material = reinterpret_cast<ParticleMaterial*>(instance->getMaterial());

        auto materialDescriptor = material->getDescriptorSet();

        animTimer += frameTimer * 0.04f;
        if (animTimer > 1.f)
        {
            animTimer = 0.f;
        }

        {
            auto cursor = platformContext->getWindow()->getInput()->getPosition();

            float deltaT = frameTimer * 2.5f;

            float destX = (cursor.first - static_cast<float>(1024 / 2)) / static_cast<float>(1024 / 2);
            float destY = (cursor.second - static_cast<float>(1024 / 2)) / static_cast<float>(1024 / 2);

            try(material->updateUniformBuffer(platformContext, deltaT, destX, destY));
        }

        rhi::ShaderParameters* shaderParameters = material->getShaderParameters();
        shaderParameters->materialDescriptor = materialDescriptor;

        rhi::ComputePipelineState computePipelineState;
        computePipelineState.shaderParameters = shaderParameters;

        context->createComputePipeline(computePipelineState);

        materialDescriptor->bind(context, 0);

        context->dispatch(PARTICLE_COUNT / 256, 1, 1);

        context->flush();
    }

    // graphics
    {
        ParticlePushBlock pushBlock;
        pushBlock.width = 1024;
        pushBlock.height = 1024;

        rhi::RenderPassInfo renderpassInfo;
        renderpassInfo.name = "Particle Graphics Pass";

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

        rhi::GraphicsPipelineState graphicsPipelineState;
        graphicsPipelineState.colorBlendState.attachmentCount = 1;
        graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].colorWriteMask = 0xF;
        graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].blendEnable = true;
        graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].colorBlendOp = rhi::BlendOp::ADD;
        graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].srcColorBlendFactor = rhi::BlendFactor::ONE;
        graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].dstColorBlendFactor = rhi::BlendFactor::ONE;
        graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].alphaBlendOp = rhi::BlendOp::ADD;
        graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].srcAlphaBlendFactor =
            rhi::BlendFactor::SRC_ALPHA;
        graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].dstAlphaBlendFactor =
            rhi::BlendFactor::DST_ALPHA;

        graphicsPipelineState.assemblyState.primitiveTopology = rhi::PrimitiveTopology::POINT_LIST;
        graphicsPipelineState.assemblyState.primitiveRestartEnable = false;
        graphicsPipelineState.rasterizationState.polygonState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
        graphicsPipelineState.rasterizationState.polygonState.polygonMode = rhi::PolygonMode::FILL;
        graphicsPipelineState.rasterizationState.polygonState.cullMode = rhi::CullMode::NONE;
        graphicsPipelineState.depthStencilState.depthTestEnable = false;
        graphicsPipelineState.depthStencilState.depthCompareOp = rhi::CompareOp::ALWAYS;
        graphicsPipelineState.depthStencilState.depthWriteEnable = false;

        graphicsPipelineState.pushConstants.push_back(
            rhi::PushConstant(rhi::ShaderStage::Vertex, 0, sizeof(ParticlePushBlock)));

        model::Material* material = graphicsInstance->getMaterial();
        auto materialDescriptor = material->getDescriptorSet();

        rhi::ShaderParameters* shaderParameters = material->getShaderParameters();
        shaderParameters->materialDescriptor = materialDescriptor;

        graphicsPipelineState.shaderParameters = shaderParameters;

        context->createGfxPipeline(graphicsPipelineState);

        context->pushConstant(rhi::ShaderStage::Vertex, sizeof(ParticlePushBlock), &pushBlock);
        materialDescriptor->bind(context, 0);

        graphicsObject->draw(context);
        graphicsInstance->draw(context);

        try(context->endRenderpass());
    }

    return Result::Continue;
}
} // namespace renderer
