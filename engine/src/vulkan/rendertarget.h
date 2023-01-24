#pragma once

#include "rhi/rendertarget.h"
#include "vulkan/core.h"
#include <mutex>
#include <unordered_map>

namespace vk
{
class Context;
class Renderpass;
class Framebuffer;

class RenderTargetManager
{
  public:
    RenderTargetManager();

    ~RenderTargetManager() = default;

    void terminate(VkDevice device);

    Result begin(Context* context, rhi::RenderPassInfo& renderpassInfo);

  private:
    size_t generateRenderpassHash(rhi::RenderPassInfo& renderpassInfo);

    size_t generateRenderpassCompatibleHash(rhi::RenderPassInfo& renderpassInfo);

    size_t generateFramebufferHash(rhi::RenderPassInfo& renderpassInfo);

  private:
    std::unordered_map<size_t, Renderpass*> renderpassMap;
    std::unordered_map<size_t, std::vector<Framebuffer*>> framebufferMap;
    std::mutex hashMutex;
};
} // namespace vk