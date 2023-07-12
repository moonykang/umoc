#pragma once

namespace renderer
{
enum class SceneRendering
{
    Forward,
    Deferred
};

class Option
{
  public:
    Option() : sceneRendering(SceneRendering::Forward), ssao(false)
    {
    }

    void enableForwardRendering()
    {
        sceneRendering = SceneRendering::Forward;
    }

    bool useForwardRendering()
    {
        return sceneRendering == SceneRendering::Forward;
    }

    void enableDeferredRendering()
    {
        sceneRendering = SceneRendering::Deferred;
    }

    bool useDeferredRendering()
    {
        return sceneRendering == SceneRendering::Deferred;
    }

    void enableSSAO()
    {
        ssao = true;
    }

    bool useSSAO()
    {
        return ssao;
    }

    void enableBloom()
    {
        bloom = true;
    }

    bool useBloom()
    {
        return bloom;
    }

  private:
    SceneRendering sceneRendering;
    bool ssao;
    bool bloom;
};
} // namespace renderer