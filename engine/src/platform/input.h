#pragma once

#include "common/util.h"
#include <mutex>

namespace scene
{
class View;
}

namespace platform
{
class Input
{
  public:
    Input();

    virtual ~Input() = default;

    Result attach(scene::View* view);

    void dettach();

    bool isAttached();

  private:
    std::mutex mutex;

  protected:
    scene::View* attachedView;
};
} // namespace platform