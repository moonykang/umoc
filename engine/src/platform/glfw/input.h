#pragma once

#include "common/singleton.h"
#include "common/util.h"
#include "platform/input.h"

namespace platform
{
namespace glfw
{
class Input : public Singleton<Input>, public platform::Input
{
  public:
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    void setEvent(GLFWwindow* window);

    void keyEvent(int key, int scancode, int action, int mods);

    void mouseEvent(int button, int action, int mods);

    void mouseCursorEvent(float x, float y);
};
} // namespace glfw
} // namespace platform