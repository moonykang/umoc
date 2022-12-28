#include "vulkan/debug.h"
#include "vulkan/core.h"
#include <sstream>

namespace vk
{
namespace debug
{
VkDebugUtilsMessengerEXT debugUtilsMessenger;

VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                           VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                           const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                           void* pUserData)
{
    // Select prefix depending on flags passed to the callback
    std::string prefix("");

    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    {
        prefix = "VERBOSE: ";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        prefix = "INFO: ";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        prefix = "WARNING: ";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        prefix = "ERROR: ";
    }

    // Display message to default output (console/logcat)
    std::stringstream debugMessage;
    debugMessage << prefix << "[" << pCallbackData->messageIdNumber << "][" << pCallbackData->pMessageIdName
                 << "] : " << pCallbackData->pMessage;

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        LOGE("%s", debugMessage.str().c_str());
    }
    else
    {
        LOGD("%s", debugMessage.str().c_str());
    }

    // The return value of this callback controls whether the Vulkan call that caused the validation message will be
    // aborted or not We return VK_FALSE as we DON'T want Vulkan calls that cause a validation message to abort If you
    // instead want to have calls abort, pass in VK_TRUE and the function will return VK_ERROR_VALIDATION_FAILED_EXT
    return VK_FALSE;
}

Result DebugUtilsMessenger::init(VkInstance instance, VkDebugReportFlagsEXT flags, VkDebugReportCallbackEXT callBack)
{
    ASSERT(!valid());

    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {};
    debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsMessengerCreateInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugUtilsMessengerCreateInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debugUtilsMessengerCreateInfo.pfnUserCallback = debugUtilsMessengerCallback;

    vk_try(vkCreateDebugUtilsMessengerEXT(instance, &debugUtilsMessengerCreateInfo, nullptr, &mHandle));
    return Result::Continue;
}

void DebugUtilsMessenger::terminate(VkInstance instance)
{
    if (valid())
    {
        vkDestroyDebugUtilsMessengerEXT(instance, mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }
}

std::string getVkResultString(VkResult result)
{
    switch (result)
    {
#define VK_RESULT_STRING(result)                                                                                       \
    case result:                                                                                                       \
        return #result

        VK_RESULT_STRING(VK_SUCCESS);
        VK_RESULT_STRING(VK_NOT_READY);
        VK_RESULT_STRING(VK_EVENT_SET);
        VK_RESULT_STRING(VK_EVENT_RESET);
        VK_RESULT_STRING(VK_INCOMPLETE);
        VK_RESULT_STRING(VK_ERROR_OUT_OF_HOST_MEMORY);
        VK_RESULT_STRING(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        VK_RESULT_STRING(VK_ERROR_INITIALIZATION_FAILED);
        VK_RESULT_STRING(VK_ERROR_DEVICE_LOST);
        VK_RESULT_STRING(VK_ERROR_MEMORY_MAP_FAILED);
        VK_RESULT_STRING(VK_ERROR_LAYER_NOT_PRESENT);
        VK_RESULT_STRING(VK_ERROR_EXTENSION_NOT_PRESENT);
        VK_RESULT_STRING(VK_ERROR_FEATURE_NOT_PRESENT);
        VK_RESULT_STRING(VK_ERROR_INCOMPATIBLE_DRIVER);
        VK_RESULT_STRING(VK_ERROR_TOO_MANY_OBJECTS);
        VK_RESULT_STRING(VK_ERROR_FORMAT_NOT_SUPPORTED);
        VK_RESULT_STRING(VK_ERROR_FRAGMENTED_POOL);
        VK_RESULT_STRING(VK_ERROR_UNKNOWN);
        VK_RESULT_STRING(VK_ERROR_OUT_OF_POOL_MEMORY);
        VK_RESULT_STRING(VK_ERROR_INVALID_EXTERNAL_HANDLE);
        VK_RESULT_STRING(VK_ERROR_FRAGMENTATION);
        VK_RESULT_STRING(VK_PIPELINE_COMPILE_REQUIRED);
        VK_RESULT_STRING(VK_ERROR_SURFACE_LOST_KHR);
        VK_RESULT_STRING(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
        VK_RESULT_STRING(VK_SUBOPTIMAL_KHR);
        VK_RESULT_STRING(VK_ERROR_OUT_OF_DATE_KHR);
#ifdef VK_KHR_display_swapchain
        VK_RESULT_STRING(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
#endif
#ifdef VK_EXT_debug_report
        VK_RESULT_STRING(VK_ERROR_VALIDATION_FAILED_EXT);
#endif
#ifdef VK_NV_glsl_shader
        VK_RESULT_STRING(VK_ERROR_INVALID_SHADER_NV);
#endif
#ifdef VK_EXT_image_drm_format_modifier
        VK_RESULT_STRING(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
#endif
#ifdef VK_KHR_global_priority
        VK_RESULT_STRING(VK_ERROR_NOT_PERMITTED_KHR);
#endif
#ifdef VK_EXT_full_screen_exclusive
        VK_RESULT_STRING(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
#endif
#ifdef VK_KHR_deferred_host_operations
        VK_RESULT_STRING(VK_THREAD_IDLE_KHR);
        VK_RESULT_STRING(VK_THREAD_DONE_KHR);
        VK_RESULT_STRING(VK_OPERATION_DEFERRED_KHR);
        VK_RESULT_STRING(VK_OPERATION_NOT_DEFERRED_KHR);
#endif
#ifdef VK_EXT_image_compression_control
        VK_RESULT_STRING(VK_ERROR_COMPRESSION_EXHAUSTED_EXT);
#endif
#ifdef VK_EXT_global_priority
        VK_RESULT_STRING(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT);
#endif

#undef VK_RESULT_STRING
    default:
        return "Unkown VkResult";
    }
}
} // namespace debug
} // namespace vk