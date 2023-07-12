#include "option.h"

namespace renderer
{
Option::Option() : sceneRendering(SceneRendering::Forward), ssao(false), finalTarget(nullptr)
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

void Option::setFinalTarget(rhi::Texture* target)
{
    finalTarget = target;
}

rhi::Texture* Option::getFinalTarget()
{
    return finalTarget;
}
} // namespace renderer