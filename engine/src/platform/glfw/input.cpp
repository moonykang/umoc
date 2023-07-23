#include "input.h"
#include "scene/view.h"

namespace platform
{
namespace glfw
{
Input::Input() : cursor({0, 0})
{
}

void Input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::GetInstance()->keyEvent(key, scancode, action, mods);
}

void Input::setEvent(GLFWwindow* window)
{
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
}

void Input::keyEvent(int key, int scancode, int action, int mods)
{
    if (!isAttached())
    {
        return;
    }

    switch (key)
    {
    case GLFW_KEY_W:
        attachedView->handle_key_W(action == GLFW_PRESS);
        break;
    case GLFW_KEY_S:
        attachedView->handle_key_S(action == GLFW_PRESS);
        break;
    case GLFW_KEY_A:
        attachedView->handle_key_A(action == GLFW_PRESS);
        break;
    case GLFW_KEY_D:
        attachedView->handle_key_D(action == GLFW_PRESS);
        break;
    default:
        LOGD("key %d scancode %d action %d mods %d", key, scancode, action, mods);
        break;
    }
}

void Input::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    Input::GetInstance()->mouseCursorEvent(static_cast<float>(xpos), static_cast<float>(ypos));
}

void Input::mouseCursorEvent(float x, float y)
{
    if (!isAttached())
    {
        return;
    }

    attachedView->handle_mouse_move(x, y);
    cursor = {x, y};
}

const std::pair<float, float>& Input::getPosition()
{
    return cursor;
}

void Input::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Input::GetInstance()->mouseEvent(button, action, mods);
}

void Input::mouseEvent(int button, int action, int mods)
{
    if (!isAttached())
    {
        return;
    }

    switch (button)
    {
    case GLFW_MOUSE_BUTTON_RIGHT:
        LOGD("GLFW_MOUSE_BUTTON_RIGHT action %d", action);
        attachedView->handle_mouse_RB(action == GLFW_PRESS);
        break;
    case GLFW_MOUSE_BUTTON_LEFT:
        LOGD("GLFW_MOUSE_BUTTON_LEFT action %d", action);
        attachedView->handle_mouse_LB(action == GLFW_PRESS);
        break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        LOGD("GLFW_MOUSE_BUTTON_MIDDLE action %d", action);
        break;
    default:
        break;
    }
}
} // namespace glfw
} // namespace platform