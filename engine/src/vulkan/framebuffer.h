#pragma once

#include "rhi/rendertarget.h"
#include "vulkan/core.h"
#include "vulkan/resources.h"

namespace vk
{
class Renderpass;

class Framebuffer final : public WrappedObject<Framebuffer, VkFramebuffer>
{
  public:
    Framebuffer();

    ~Framebuffer() = default;

    Result init(Context* context, rhi::RenderPassInfo& renderpassInfo, Renderpass* renderpass, size_t hash);

    void terminate(VkDevice device);

    bool match(size_t hash);

  private:
    VkResult create(VkDevice device, const VkFramebufferCreateInfo& createInfo);

  private:
    size_t framebufferHash;
};
} // namespace vk