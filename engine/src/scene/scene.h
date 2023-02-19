#pragma once

#include "common/util.h"

namespace platform
{
class Conetxt;
}

namespace scene
{
class Scene
{
  public:
    virtual Result init(platform::Conetxt* context) = 0;

    virtual Result load(platform::Conetxt* context) = 0;

    virtual Result udpate(platform::Conetxt* context) = 0;

    virtual void terminate(platform::Conetxt* context) = 0;

  protected:
};
} // namespace scene