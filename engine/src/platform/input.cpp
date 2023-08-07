#include "input.h"
#include "scene/view.h"

namespace platform
{
Input::Input() : attachedView(nullptr)
{
}

Result Input::attach(scene::View* view)
{
    std::lock_guard<std::mutex> lock(mutex);
    attachedView = view;

    return Result::Continue;
}

void Input::dettach()
{
    std::lock_guard<std::mutex> lock(mutex);
    attachedView = nullptr;
}

bool Input::isAttached()
{
    std::lock_guard<std::mutex> lock(mutex);
    return attachedView != nullptr;
}

input::MouseStatus& Input::getMouseStatus()
{
    return mouseStatus;
}
} // namespace platform