#include "nbody.h"
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

#define PARTICLES_PER_ATTRACTOR 4 * 1024

namespace renderer
{

namespace nbody
{
struct Particle
{
    glm::vec4 pos;
    glm::vec4 vel;
};

struct ComputeUBO
{
    float deltaT;
    int32_t particleCount;
};

struct PushBlock
{
    float width;
    float height;
} pushBlock;
} // namespace nbody

Result NBodyPass::init(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    std::vector<glm::vec3> attractors = {
        glm::vec3(5.0f, 0.0f, 0.0f),  glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 5.0f),
        glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 4.0f, 0.0f),  glm::vec3(0.0f, -8.0f, 0.0f),
    };

    numParticles = static_cast<uint32_t>(attractors.size()) * PARTICLES_PER_ATTRACTOR;

    // Initial particle positions
    std::vector<nbody::Particle> particleBuffer(numParticles);
    //
    std::default_random_engine rndEngine(0); // benchmark.active ? 0 : (unsigned)time(nullptr));
    std::normal_distribution<float> rndDist(0.0f, 1.0f);

    for (uint32_t i = 0; i < static_cast<uint32_t>(attractors.size()); i++)
    {
        for (uint32_t j = 0; j < PARTICLES_PER_ATTRACTOR; j++)
        {
            nbody::Particle& particle = particleBuffer[i * PARTICLES_PER_ATTRACTOR + j];

            // First particle in group as heavy center of gravity
            if (j == 0)
            {
                particle.pos = glm::vec4(attractors[i] * 1.5f, 90000.0f);
                particle.vel = glm::vec4(glm::vec4(0.0f));
            }
            else
            {
                // Position
                glm::vec3 position(attractors[i] +
                                   glm::vec3(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine)) * 0.75f);
                float len = glm::length(glm::normalize(position - attractors[i]));
                position.y *= 2.0f - (len * len);

                // Velocity
                glm::vec3 angular = glm::vec3(0.5f, 1.5f, 0.5f) * (((i % 2) == 0) ? 1.0f : -1.0f);
                glm::vec3 velocity = glm::cross((position - attractors[i]), angular) +
                                     glm::vec3(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine) * 0.025f);

                float mass = (rndDist(rndEngine) * 0.5f + 0.5f) * 75.0f;
                particle.pos = glm::vec4(position, mass);
                particle.vel = glm::vec4(velocity, 0.0f);
            }

            // Color gradient offset
            particle.vel.w = (float)i * 1.0f / static_cast<uint32_t>(attractors.size());
        }
    }

    size_t storageBufferSize = particleBuffer.size() * sizeof(nbody::Particle);

    computeStorageBuffer = context->allocateStorageBuffer(storageBufferSize, particleBuffer.data());

    nbody::ComputeUBO computeUBO;
    computeUBO.deltaT = 0;
    computeUBO.particleCount = numParticles;

    computeUniformBuffer = context->allocateUniformBuffer(sizeof(nbody::ComputeUBO), &computeUBO);

    // Calculate Compute
    {
        model::Material* material = new model::Material();
        try(material->init(platformContext));
        material->updateUniformBuffer(computeUniformBuffer, rhi::ShaderStage::Compute);
        material->updateStorageBuffer(computeStorageBuffer, rhi::ShaderStage::Compute);
        try(material->update(platformContext));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.computeShader = context->allocateComputeShader("nbody/calculate.comp.spv");

        auto loader =
            model::predefined::Loader::Builder().setMaterial(material).setShaderParameters(&shaderParameters).build();

        calculateObject = loader->load(platformContext, sceneInfo);
        calculateInstance = calculateObject->instantiate(platformContext, glm::mat4(1.0f), true);
    }

    // Integrate Compute
    {
        model::Material* material = new model::Material();
        try(material->init(platformContext));
        material->updateUniformBuffer(computeUniformBuffer, rhi::ShaderStage::Compute);
        material->updateStorageBuffer(computeStorageBuffer, rhi::ShaderStage::Compute);
        try(material->update(platformContext));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.computeShader = context->allocateComputeShader("nbody/integrate.comp.spv");

        auto loader =
            model::predefined::Loader::Builder().setMaterial(material).setShaderParameters(&shaderParameters).build();

        integrateObject = loader->load(platformContext, sceneInfo);
        integrateInstance = integrateObject->instantiate(platformContext, glm::mat4(1.0f), true);
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
            rhi::VertexAttribute(rhi::Format::R32G32B32A32_FLOAT, offsetof(nbody::Particle, pos)),
            rhi::VertexAttribute(rhi::Format::R32G32B32A32_FLOAT, offsetof(nbody::Particle, vel))};

        shaderParameters.vertexShader =
            context->allocateVertexShader("nbody/particle.vert.spv", vertexAttributes, sizeof(nbody::Particle));
        shaderParameters.pixelShader = context->allocatePixelShader("nbody/particle.frag.spv");

        auto loader = model::predefined::Loader::Builder()
                          .setPredefineModelType(model::PredefinedModel::Storage)
                          .setMaterial(material)
                          .setShaderParameters(&shaderParameters)
                          .setExternalVertexBuffer({computeStorageBuffer, PARTICLE_COUNT})
                          .build();

        graphicsObject = loader->load(platformContext, sceneInfo);
        graphicsInstance = graphicsObject->instantiate(platformContext, glm::mat4(1.0f), true);
    }

    return Result::Continue;
}

void NBodyPass::terminate(platform::Context* context)
{
    TERMINATE(calculateObject, context);
    TERMINATE(integrateObject, context);
    TERMINATE(graphicsObject, context);
}

Result NBodyPass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    {
        float frameTimer = platformContext->getTimer().getCurrentFrameTimer();

        nbody::ComputeUBO computeUBO;
        computeUBO.particleCount = numParticles;
        computeUBO.deltaT = frameTimer * 0.05;

        computeUniformBuffer->update(context, sizeof(nbody::ComputeUBO), &computeUBO);
    }

    // calculate
    {
        auto material = calculateInstance->getMaterial();

        auto materialDescriptor = material->getDescriptorSet();

        rhi::ShaderParameters* shaderParameters = material->getShaderParameters();
        shaderParameters->materialDescriptor = materialDescriptor;

        rhi::ComputePipelineState computePipelineState;
        computePipelineState.shaderParameters = shaderParameters;

        context->createComputePipeline(computePipelineState);

        materialDescriptor->bind(context, 0);

        context->dispatch(PARTICLE_COUNT / 256, 1, 1);

        context->flush();
    }

    // integreate
    {
        // buffer transition

        try(context->addTransition(computeStorageBuffer->getBuffer(), computeStorageBuffer->getOffset(),
                                   computeStorageBuffer->getSize(), rhi::ImageLayout::ComputeShaderWrite,
                                   rhi::ImageLayout::ComputeShaderReadOnly));
        try(context->flushTransition());

        auto material = integrateInstance->getMaterial();

        auto materialDescriptor = material->getDescriptorSet();

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
        nbody::PushBlock pushBlock;
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
            rhi::PushConstant(rhi::ShaderStage::Vertex, 0, sizeof(nbody::PushBlock)));

        model::Material* material = graphicsInstance->getMaterial();
        auto materialDescriptor = material->getDescriptorSet();

        rhi::ShaderParameters* shaderParameters = material->getShaderParameters();
        shaderParameters->globalDescriptor = sceneInfo->getDescriptorSet();
        shaderParameters->localDescriptor = graphicsInstance->getDescriptorSet();
        shaderParameters->materialDescriptor = materialDescriptor;

        graphicsPipelineState.shaderParameters = shaderParameters;

        context->createGfxPipeline(graphicsPipelineState);

        context->pushConstant(rhi::ShaderStage::Vertex, sizeof(nbody::PushBlock), &pushBlock);

        sceneInfo->getDescriptorSet()->bind(context, 0);
        graphicsInstance->getDescriptorSet()->bind(context, 1);
        materialDescriptor->bind(context, 2);

        graphicsObject->draw(context);
        graphicsInstance->draw(context);

        try(context->endRenderpass());
    }

    return Result::Continue;
}
} // namespace renderer
