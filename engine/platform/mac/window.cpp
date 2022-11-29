#include "platform/mac/window.h"

// TODO
#define WIDTH 1024
#define HEIGHT 1024

namespace platform
{
namespace mac
{
Window::Window() : window(nullptr)
{
}

void Window::init()
{
    LOGD("Init Mac platform window");

    initGlfw();
    initRHI();
}

void Window::initGlfw()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void Window::terminate()
{
    LOGD("Terminate Mac platform window");

    glfwDestroyWindow(window);
    glfwTerminate();
}

} // namespace mac
} // namespace platform