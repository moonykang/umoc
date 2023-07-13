#pragma once

namespace rhi
{
class Texture;
}

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
    Option();

    void enableForwardRendering();

    bool useForwardRendering();

    void enableDeferredRendering();

    bool useDeferredRendering();

    void enableSSAO();

    bool useSSAO();

    void enableBloom();

    bool useBloom();

    void enableEnvironmap();

    bool useEnvironmap();

    void setFinalTarget(rhi::Texture* target);

    rhi::Texture* getFinalTarget();

  private:
    SceneRendering sceneRendering;
    bool ssao;
    bool bloom;
    bool environmap;

    rhi::Texture* finalTarget;
};
} // namespace renderer