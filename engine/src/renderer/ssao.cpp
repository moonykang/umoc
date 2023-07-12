#include "ssao.h"
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
#include "scene/textures.h"
#include <random>

namespace renderer
{
#define NUM_SSAO_KERNEL 64
#define SSAO_NOISE_DIM 4

class SSAOMaterial : public model::Material
{
  public:
    struct SSAOKernelSample
    {
        glm::vec4 samples[NUM_SSAO_KERNEL];
        uint32_t kernel_size;
        float radius;
    } ubo;

    struct SSAONoize
    {

        ALIGNED(32)
        double r;
        ALIGNED(32)
        double g;
        ALIGNED(32)
        double b;
        ALIGNED(32)
        double a;
    };

    Result init(platform::Context* platformContext) override
    {
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
        uniformBuffer = context->allocateUniformBuffer(sizeof(SSAOKernelSample), &ubo);

        return model::Material::init(platformContext);
    }

    Result update(platform::Context* platformContext) override
    {
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
        try(uniformBuffer->update(context, sizeof(SSAOKernelSample), &ubo));

        return model::Material::update(platformContext);
    }

    void initSSAOKernelSamples(rhi::Context* context, scene::SceneInfo* sceneInfo)
    {
        ubo.kernel_size = NUM_SSAO_KERNEL;
        ubo.radius = 0.5f;

        std::default_random_engine rndEngine(0); // TODO: Seed
        std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

        auto lerp = [](float a, float b, float f) { return a + f * (b - a); };

        for (uint32_t i = 0; i < NUM_SSAO_KERNEL; ++i)
        {
            glm::vec3 sample(rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine));
            sample = glm::normalize(sample);
            sample *= rndDist(rndEngine);
            float scale = float(i) / float(NUM_SSAO_KERNEL);
            scale = lerp(0.1f, 1.0f, scale * scale);
            ubo.samples[i] = glm::vec4(sample * scale, 0.0f);
        }

        std::vector<SSAONoize> ssaoNoise(SSAO_NOISE_DIM * SSAO_NOISE_DIM);
        for (uint32_t i = 0; i < static_cast<uint32_t>(ssaoNoise.size()); i++)
        {
            glm::vec4 temp = glm::vec4(rndDist(rndEngine) * 2.0f - 1.0f, rndDist(rndEngine) * 2.0f - 1.0f, 0.0f, 0.0f);
            ssaoNoise[i].r = temp.r;
            ssaoNoise[i].g = temp.g;
            ssaoNoise[i].b = temp.b;
            ssaoNoise[i].a = temp.a;
        }

        auto [id, texture] = sceneInfo->getTextures()->get(context, "SSAO Noise", rhi::Format::R32G32B32A32_FLOAT,
                                                           {SSAO_NOISE_DIM, SSAO_NOISE_DIM, 1},
                                                           sizeof(SSAONoize) * ssaoNoise.size(), ssaoNoise.data());
        noiseTextureId = id;
    }

    SSAOKernelSample& getKernelSampleUBO()
    {
        return ubo;
    }

    model::TextureID noiseTextureId;
};

Result SSAOPass::init(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    // setup
    {
        SSAOMaterial* material = new SSAOMaterial();
        try(material->init(platformContext));
        material->initSSAOKernelSamples(context, sceneInfo);
        material->updateTexture(model::MaterialFlag::External, sceneInfo->getRenderTargets()->getGBufferA()); // pos
        material->updateTexture(model::MaterialFlag::External, sceneInfo->getRenderTargets()->getGBufferB()); // normal
        material->updateTexture(model::MaterialFlag::External, sceneInfo->getTextures()->get(material->noiseTextureId));
        try(material->update(platformContext));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "screen/screen.vert.spv",
            rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal);
        shaderParameters.pixelShader = context->allocatePixelShader("ssao/ssao.frag.spv");

        auto loader =
            model::predefined::Loader::Builder().setMaterial(material).setShaderParameters(&shaderParameters).build();

        object = loader->load(platformContext, sceneInfo);
        instance = object->instantiate(platformContext, glm::mat4(1.0f), true);
    }

    return Result::Continue;
}

void SSAOPass::terminate(platform::Context* context)
{
    TERMINATE(object, context);
}

Result SSAOPass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    // setup
    {
        rhi::RenderPassInfo renderpassInfo;
        renderpassInfo.name = "SSAO Pass";

        rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
            {sceneInfo->getRenderTargets()->getSSAO()->getImage(), rhi::AttachmentLoadOp::Clear,
             rhi::AttachmentStoreOp::Store, 1, rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

        auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
        subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});

        auto material = reinterpret_cast<SSAOMaterial*>(instance->getMaterial());

        try(context->addTransition(sceneInfo->getRenderTargets()->getSSAO()->getImage(),
                                   rhi::ImageLayout::ColorAttachment));
        try(context->addTransition(sceneInfo->getRenderTargets()->getGBufferA()->getImage(),
                                   rhi::ImageLayout::FragmentShaderReadOnly));
        try(context->addTransition(sceneInfo->getRenderTargets()->getGBufferB()->getImage(),
                                   rhi::ImageLayout::FragmentShaderReadOnly));
        try(context->addTransition(sceneInfo->getTextures()->get(material->noiseTextureId)->getImage(),
                                   rhi::ImageLayout::FragmentShaderReadOnly));
        try(context->beginRenderpass(renderpassInfo));

        auto materialDescriptor = material->getDescriptorSet();

        rhi::ShaderParameters* shaderParameters = material->getShaderParameters();
        shaderParameters->globalDescriptor = sceneInfo->getDescriptorSet();
        shaderParameters->materialDescriptor = materialDescriptor;

        rhi::GraphicsPipelineState graphicsPipelineState;
        graphicsPipelineState.shaderParameters = shaderParameters;
        graphicsPipelineState.colorBlendState.attachmentCount = 1;
        graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
        graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::FRONT_BIT;

        context->createGfxPipeline(graphicsPipelineState);

        sceneInfo->getDescriptorSet()->bind(context, 0);
        materialDescriptor->bind(context, 1);

        object->draw(context);
        instance->draw(context);

        try(context->endRenderpass());
    }

    return Result::Continue;
}
} // namespace renderer
