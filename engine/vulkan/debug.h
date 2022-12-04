#pragma once

#include "vulkan/wrapper.h"
#include <string>

namespace vk
{
namespace debug
{
std::string getVkResultString(VkResult result);

}
} // namespace vk