#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#if PLATFORM_WINDOW
#define GLFW_EXPOSE_NATIVE_WIN32
#elif PLATFORM_MAC
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <GLFW/glfw3native.h>