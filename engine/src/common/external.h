#pragma once

// GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#if PLATFORM_WINDOW
#define GLFW_EXPOSE_NATIVE_WIN32
#elif PLATFORM_MAC
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <GLFW/glfw3native.h>

// Tiny GLTF
#define TINYGLTF_NO_STB_IMAGE_WRITE
#ifdef PLATFORM_ANDROID
#define TINYGLTF_ANDROID_LOAD_FROM_ASSETS
#endif
#include "tiny_gltf.h"

// ImGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "ImGuizmo.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#ifdef near
#undef near
#endif

#ifdef far
#undef far
#endif