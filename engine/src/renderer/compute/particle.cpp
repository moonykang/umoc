#include "particle.h"
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
#include <random>

#define PARTICLE_COUNT 256 * 1024

namespace renderer
{
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

    struct Particle
    {
        glm::vec2 pos;
        glm::vec2 vel;
        glm::vec4 gradientPos;
    };

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

        updateStorageBuffer(storageBuffer, rhi::ShaderStage::Compute);

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

    // setup
    {
        ParticleMaterial* material = new ParticleMaterial();
        try(material->init(platformContext));
        try(material->update(platformContext));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.computeShader = context->allocateComputeShader("particle/particle.comp.spv");

        auto loader =
            model::predefined::Loader::Builder().setMaterial(material).setShaderParameters(&shaderParameters).build();

        object = loader->load(platformContext, sceneInfo);
        instance = object->instantiate(platformContext, glm::mat4(1.0f), true);
    }

    return Result::Continue;
}

void ParticlePass::terminate(platform::Context* context)
{
    TERMINATE(object, context);
}

Result ParticlePass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    uint64_t time = platformContext->getTimer().getCurrentTime();

    ParticleMaterial* material = reinterpret_cast<ParticleMaterial*>(instance->getMaterial());

    auto materialDescriptor = material->getDescriptorSet();
    {
        float deltaT = time * 2.5f;
        float destX = sin(glm::radians(time * 360.0f)) * 0.75f;
        float destY = 0.0f;
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

    return Result::Continue;
}
} // namespace renderer