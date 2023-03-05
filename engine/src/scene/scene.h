#pragma once

#include "common/util.h"
#include <vector>

namespace platform
{
class Context;
}

namespace model
{
class VertexInput;
}

namespace scene
{
class View;
class SceneInfo
{
  public:
    SceneInfo();

    virtual ~SceneInfo() = default;

    Result init(platform::Context* context);

    virtual Result postInit(platform::Context* context) = 0;

    virtual Result load(platform::Context* context) = 0;

    virtual Result udpate(platform::Context* context) = 0;

    virtual void preTerminate(platform::Context* context) = 0;

    void terminate(platform::Context* context);

    virtual void postTerminate(platform::Context* context) = 0;

    std::vector<model::VertexInput*>& getModels()
    {
        return models;
    }

    View* getView()
    {
        return view;
    }

  protected:
    model::VertexInput* registerObject(platform::Context* context, model::VertexInput* object);

  protected:
    std::vector<model::VertexInput*> models;
    View* view;
};
} // namespace scene