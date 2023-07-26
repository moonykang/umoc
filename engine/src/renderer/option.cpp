#include "option.h"

namespace renderer
{
Option::Option()
    : sceneRendering(SceneRendering::Forward), ssao(false), finalTarget(nullptr),
      computePostProcess(ComputePostProcess::None)
{
}

void Option::enableForwardRendering()
{
    sceneRendering = SceneRendering::Forward;
}

bool Option::useForwardRendering()
{
    return sceneRendering == SceneRendering::Forward;
}

void Option::enableDeferredRendering()
{
    sceneRendering = SceneRendering::Deferred;
}

bool Option::useDeferredRendering()
{
    return sceneRendering == SceneRendering::Deferred;
}

void Option::enableParticleRendering()
{
    sceneRendering = SceneRendering::Particle;
}

bool Option::useParticleRendering()
{
    return sceneRendering == SceneRendering::Particle;
}

void Option::enableSSAO()
{
    ssao = true;
}

bool Option::useSSAO()
{
    return ssao;
}

void Option::enableBloom()
{
    bloom = true;
}

bool Option::useBloom()
{
    return bloom;
}

void Option::enableEnvironmap()
{
    environmap = true;
}

bool Option::useEnvironmap()
{
    return environmap;
}

void Option::setFinalTarget(rhi::Texture* target)
{
    finalTarget = target;
}

rhi::Texture* Option::getFinalTarget()
{
    return finalTarget;
}

void Option::setComputePostProcess(ComputePostProcess computePostProcess)
{
    this->computePostProcess = computePostProcess;
}

ComputePostProcess Option::getComputePostProcess()
{
    return computePostProcess;
}
} // namespace renderer