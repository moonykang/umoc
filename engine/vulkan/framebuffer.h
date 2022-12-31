#pragma once

#include "vulkan/core.h"

namespace vk
{
class Framebuffer final : public WrappedObject<Framebuffer, VkFramebuffer>
{
};
} // namespace vk