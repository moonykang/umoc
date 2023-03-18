#include "platform/glfw/window.h"
#include "input.h"

// TODO
#define WIDTH 1024
#define HEIGHT 1024

namespace platform
{

Window* Window::createPlatformWindow()
{
    return new glfw::Window();
}

namespace glfw
{
Window::Window() : window(nullptr)
{
}

void Window::init()
{
    LOGD("Init Mac platform window");

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    Input::GetInstance()->setEvent(window);
}

void Window::terminate()
{
    LOGD("Terminate Mac platform window");

    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::valid()
{
    ASSERT(window);

    glfwPollEvents();

    return !glfwWindowShouldClose(window);
}

platform::Input* Window::getInput()
{
    return Input::GetInstance();
}

GLFWwindow* Window::getWindow()
{
    ASSERT(window != nullptr);
    return window;
}
} // namespace glfw
} // namespace platform