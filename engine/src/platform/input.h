#pragma once

#include "common/util.h"
#include <mutex>

namespace scene
{
class View;
}

namespace platform
{
namespace input
{
class MouseStatus
{
  public:
    MouseStatus() : left(false), right(false), middle(false)
    {
    }

    bool left;
    bool right;
    bool middle;
};
} // namespace input
class Input
{
  public:
    Input();

    virtual ~Input() = default;

    Result attach(scene::View* view);

    void dettach();

    bool isAttached();

    virtual const std::pair<float, float>& getPosition() = 0;

    input::MouseStatus& getMouseStatus();

  private:
    std::mutex mutex;

  protected:
    scene::View* attachedView;
    input::MouseStatus mouseStatus;
};
} // namespace platform