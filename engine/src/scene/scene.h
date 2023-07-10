#pragma once

#include "common/util.h"
#include <vector>

namespace platform
{
class Context;
}

namespace model
{
class Object;
}

namespace rhi
{
class DescriptorSet;
}

namespace scene
{
class Light;
class View;
class RenderTargets;
class Textures;

class SceneInfo
{
  public:
    SceneInfo();

    virtual ~SceneInfo() = default;

    Result init(platform::Context* context);

    virtual Result postInit(platform::Context* context);

    virtual Result load(platform::Context* context) = 0;

    virtual Result udpate(platform::Context* context);

    virtual void preTerminate(platform::Context* context);

    void terminate(platform::Context* context);

    virtual void postTerminate(platform::Context* context);

    std::vector<model::Object*>& getModels()
    {
        return models;
    }

    View* getView()
    {
        return view;
    }

    Light* getLight()
    {
        return light;
    }

    RenderTargets* getRenderTargets()
    {
        return renderTargets;
    }

    Result updateDescriptor(platform::Context* context);

    rhi::DescriptorSet* getDescriptorSet();

    Textures* getTextures()
    {
        return textures;
    }

  protected:
    void registerObject(platform::Context* context, model::Object* object);

  protected:
    std::vector<model::Object*> models;
    View* view;
    Light* light;
    RenderTargets* renderTargets;
    rhi::DescriptorSet* sceneDescriptorSet;
    Textures* textures;
    uint64_t timer;
};
} // namespace scene