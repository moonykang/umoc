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
class SceneInfo
{
  public:
    virtual ~SceneInfo() = default;

    virtual Result init(platform::Context* context) = 0;

    virtual Result load(platform::Context* context) = 0;

    virtual Result udpate(platform::Context* context) = 0;

    virtual void terminate(platform::Context* context) = 0;

    std::vector<model::VertexInput*>& getModels()
    {
        return models;
    }

  protected:
    std::vector<model::VertexInput*> models;
};
} // namespace scene