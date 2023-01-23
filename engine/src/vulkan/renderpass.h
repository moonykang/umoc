#pragma once

#include "rhi/rendertarget.h"
#include "vulkan/core.h"
#include "vulkan/resources.h"
#include <cstdlib>

namespace rhi
{
class Context;
}

namespace vk
{
class Conetxt;

class Renderpass final : public WrappedObject<Renderpass, VkRenderPass>
{
  public:
    Renderpass();

    ~Renderpass() = default;

    Result init(Context* context, rhi::RenderPassInfo& renderpassInfo, size_t hash);

    void terminate(VkDevice device);

  private:
    VkResult create(VkDevice device, const VkRenderPassCreateInfo& createInfo);

  private:
    size_t compatibleHash;
};
} // namespace vk