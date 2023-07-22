#include "filter.h"
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
namespace compute
{
Result FilterPass::init(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    // setup
    {
        model::Material* material = new model::Material();
        try(material->init(platformContext));
        material->updateTexture(model::MaterialFlag::BaseColorTexture, sceneInfo->getRenderTargets()->getSceneColor(),
                                rhi::ShaderStage::Compute);
        material->updateTexture(model::MaterialFlag::External, sceneInfo->getRenderTargets()->getComputeTarget(),
                                rhi::ShaderStage::Compute);
        try(material->update(platformContext));

        std::string computeShader;
        switch (sceneInfo->getRenderingOptions().getComputePostProcess())
        {
        case ComputePostProcess::EdgeDetection:
            computeShader = "filter/edgedetect.comp.spv";
            break;
        case ComputePostProcess::Sharpen:
            computeShader = "filter/sharpen.comp.spv";
            break;
        case ComputePostProcess::Emboss:
            computeShader = "filter/emboss.comp.spv";
            break;
        default:
            computeShader = "filter/dummy.comp.spv";
            break;
        }
        rhi::ShaderParameters shaderParameters;
        shaderParameters.computeShader = context->allocateComputeShader(computeShader);

        auto loader =
            model::predefined::Loader::Builder().setMaterial(material).setShaderParameters(&shaderParameters).build();

        object = loader->load(platformContext, sceneInfo);
        instance = object->instantiate(platformContext, glm::mat4(1.0f), true);
    }

    return Result::Continue;
}

void FilterPass::terminate(platform::Context* context)
{
    TERMINATE(object, context);
}

Result FilterPass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    uint64_t time = platformContext->getTimer().getCurrentTime();

    try(context->addTransition(sceneInfo->getRenderTargets()->getSceneColor()->getImage(),
                               rhi::ImageLayout::ComputeShaderReadOnly));
    try(context->addTransition(sceneInfo->getRenderTargets()->getComputeTarget()->getImage(),
                               rhi::ImageLayout::ComputeShaderWrite));

    try(context->flush());
    try(context->waitIdle());

    auto material = instance->getMaterial();
    auto materialDescriptor = material->getDescriptorSet();

    rhi::ShaderParameters* shaderParameters = material->getShaderParameters();
    shaderParameters->materialDescriptor = materialDescriptor;

    rhi::ComputePipelineState computePipelineState;
    computePipelineState.shaderParameters = shaderParameters;

    context->createComputePipeline(computePipelineState);

    materialDescriptor->bind(context, 0);

    context->dispatch(1024 / 16, 1024 / 16, 1);

    try(context->flush());

    return Result::Continue;
}
} // namespace compute

} // namespace renderer