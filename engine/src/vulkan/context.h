#pragma once

#include "rhi/context.h"
#include "vulkan/core.h"
#include <map>

namespace rhi
{
class Image;
class Renderpass;
class ShaderBase;
} // namespace rhi

namespace vk
{
class Device;
class DeviceExtension;
class InstanceExtension;
class Instance;
class PhysicalDevice;
class Surface;
class Swapchain;
class DebugUtilsMessenger;
class QueueMap;
class RenderTargetManager;
class CommandBuffer;
class Shader;
class ShaderMap;
class PipelineMap;
class Renderpass;

class Context : public rhi::Context
{
  public:
    Context();

    Result initRHIImplementation(platform::Window* window) override final;

    void terminateRHIImplementation() override final;

    rhi::Image* getCurrentSurfaceImage() override;

    Result flush() override;

    Result present() override;

    Result beginRenderpass(rhi::RenderPassInfo& renderpassInfo) override final;

    Result endRenderpass() override final;

    Result createGfxPipeline(rhi::GraphicsPipelineState gfxPipelineState) override final;

    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;

  public:
    Instance* getInstance() const;

    PhysicalDevice* getPhysicalDevice() const;

    Device* getDevice() const;

    Surface* getSurface() const;

    Swapchain* getSwapchain() const;

    CommandBuffer* getActiveCommandBuffer() const;

    CommandBuffer* getUploadCommandBuffer() const;

    QueueMap* getQueueMap() const;

    size_t getCurrentRenderpassHash();

    Renderpass* getCurrentRenderpass();

    Shader* getShader(rhi::ShaderBase* shaderBase);

  private:
    Instance* instance;
    Surface* surface;
    PhysicalDevice* physicalDevice;
    Device* device;
    Swapchain* swapchain;
    QueueMap* queueMap;
    ShaderMap* shaderMap;
    PipelineMap* pipelineMap;

    DebugUtilsMessenger* debugCallback;
    RenderTargetManager* renderTargetManager;

    std::map<ExtensionName, InstanceExtension*> instanceExtensions;
    std::map<ExtensionName, DeviceExtension*> deviceExtensions;

    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2;

  private:
    bool enableValidationLayer;
};
} // namespace vk