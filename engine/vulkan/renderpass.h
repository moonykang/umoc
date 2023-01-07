#pragma once

#include "vulkan/core.h"

namespace vk
{
class AttachmentDescription final : public WrappedObjectList<AttachmentDescription, VkAttachmentDescription>
{
};

class Renderpass final : public WrappedObject<Renderpass, VkRenderPass>
{
  public:
};
} // namespace vk