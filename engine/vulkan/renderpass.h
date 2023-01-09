#pragma once

#include "rhi/rendertarget.h"
#include "vulkan/core.h"
#include "vulkan/resources.h"

namespace vk
{
class Conetxt;

class AttachmentDescription final : public WrappedObjectList<AttachmentDescription, VkAttachmentDescription>
{
};

class Renderpass final : public WrappedObject<Renderpass, VkRenderPass>
{
  public:
    Result init(Context* context, rhi::RenderPassInfo& renderpassInfo);

    void terminate(VkDevice device);

  private:
    VkResult create(VkDevice device, const VkRenderPassCreateInfo& createInfo);
};
} // namespace vk