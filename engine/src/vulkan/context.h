#pragma once

#include "rhi/context.h"
#include "util.h"
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
class Buffer;
class Device;
class DeviceExtension;
class DescriptorPool;
class DescriptorSet;
class DescriptorSetLayout;
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
class PendingState;

class Context : public rhi::Context
{
  public:
    Context();

    Result initRHIImplementation(platform::Window* window) override final;

    void terminateRHIImplementation() override final;

    rhi::Image* getCurrentSurfaceImage() override;

    rhi::Extent3D getSurfaceSize() override;

    Result flush() override;

    Result present() override;

    Result waitIdle() override;

    Result beginRenderpass(rhi::RenderPassInfo& renderpassInfo) override final;

    Result endRenderpass() override final;

    Result createGfxPipeline(rhi::GraphicsPipelineState gfxPipelineState) override final;

    Result createComputePipeline(rhi::ComputePipelineState pipelineState) override final;

    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;

    void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset,
                     uint32_t firstInstance) override;

    void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;

    Result addTransition(rhi::Image* image, rhi::ImageLayout layout) override;

    Result addTransition(rhi::Buffer* buffer, size_t offset, size_t size, rhi::ImageLayout src,
                         rhi::ImageLayout dst) override;

    Result flushTransition() override;

    void pushConstant(rhi::ShaderStageFlags shaderStage, size_t size, void* data) override;

    Result copyImage(rhi::Image* srcImage, rhi::ImageSubResource srcRange, rhi::Image* dstImage,
                     rhi::ImageSubResource dstRange, rhi::Extent3D extent) override;

    Result readBackImage(rhi::Image* srcImage, rhi::ImageSubResource srcRange, void* data) override;

    Result viewport(rhi::Extent2D extent) override;
    // factory
  public:
    rhi::Buffer* allocateBuffer(rhi::BufferUsageFlags bufferUsage, rhi::MemoryPropertyFlags memoryProperty,
                                size_t size) override;

    rhi::Image* allocateImage(std::string name, rhi::DescriptorType descriptorType) override;

    rhi::DescriptorSet* allocateDescriptorSet() override;

    rhi::VertexShaderBase* createVertexShader(rhi::ResourceID id, std::string name,
                                              rhi::VertexChannelFlags vertexChannelFlags) override final;

    rhi::VertexShaderBase* createVertexShader(rhi::ResourceID id, std::string name,
                                              std::vector<rhi::VertexAttribute>& vertexAttribute,
                                              uint32_t stride) override final;

    rhi::PixelShaderBase* createPixelShader(rhi::ResourceID id, std::string name) override final;

    rhi::ComputeShaderBase* createComputeShader(rhi::ResourceID id, std::string name) override final;

  public:
    Instance* getInstance() const;

    PhysicalDevice* getPhysicalDevice() const;

    Device* getDevice() const;

    Surface* getSurface() const;

    Swapchain* getSwapchain() const;

    CommandBuffer* getActiveCommandBuffer() const;

    CommandBuffer* getUploadCommandBuffer() const;

    Result submitUploadCommandBuffer();

    Result submitActiveCommandBuffer();

    QueueMap* getQueueMap() const;

    size_t getCurrentRenderpassHash();

    Renderpass* getCurrentRenderpass();

    DescriptorPool* getDescriptorPool();

    PendingState* getPendingState();

    template <typename T> void addGarbage(HandleType type, T* object)
    {
        garbageList.emplace_back(GarbageObject(type, (GarbageHandle)object));
    }

    void clearAllGarbage();

    DescriptorSetLayout* getEmptyDescriptorSetLayout();

    Result debugMarkerSetObjectName(uint64_t object, VkDebugReportObjectTypeEXT objectType, const char* name);

    bool supportInstanceExtension(ExtensionName extensionName);

    bool supportDeviceExtension(ExtensionName extensionName);

  private:
    Result initEmptyDescriptorSetLayout();

  private:
    Instance* instance;
    Surface* surface;
    PhysicalDevice* physicalDevice;
    Device* device;
    DescriptorPool* descriptorPool;
    Swapchain* swapchain;
    QueueMap* queueMap;
    PipelineMap* pipelineMap;
    DescriptorSetLayout* emptyDescriptorSetLayout;

    DebugUtilsMessenger* debugCallback;
    RenderTargetManager* renderTargetManager;

    std::map<ExtensionName, InstanceExtension*> instanceExtensions;
    std::map<ExtensionName, DeviceExtension*> deviceExtensions;

    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2;

    GarbageList garbageList;

    PendingState* pendingState;

  private:
    bool enableValidationLayer;
};
} // namespace vk
