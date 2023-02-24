#include "util.h"
#include "context.h"
#include "core.h"
#include "device.h"
#include <utility>

namespace vk
{

GarbageObject::GarbageObject() : mHandleType(HandleType::Invalid), mHandle(VK_NULL_HANDLE)
{
}

GarbageObject::GarbageObject(HandleType handleType, GarbageHandle handle) : mHandleType(handleType), mHandle(handle)
{
}

GarbageObject::GarbageObject(GarbageObject&& other) : GarbageObject()
{
    *this = std::move(other);
}

GarbageObject& GarbageObject::operator=(GarbageObject&& rhs)
{
    std::swap(mHandle, rhs.mHandle);
    std::swap(mHandleType, rhs.mHandleType);
    return *this;
}

bool GarbageObject::valid() const
{
    return mHandle != VK_NULL_HANDLE;
}

// GarbageObject implementation
// Using c-style casts here to avoid conditional compile for MSVC 32-bit
//  which fails to compile with reinterpret_cast, requiring static_cast.
void GarbageObject::terminate(Context* context)
{
    VkDevice device = context->getDevice()->getHandle();

    switch (mHandleType)
    {
    case HandleType::Semaphore:
        vkDestroySemaphore(device, (VkSemaphore)mHandle, nullptr);
        break;
    case HandleType::CommandBuffer:
        // Command buffers are pool allocated.
        UNREACHABLE();
        break;
    case HandleType::Event:
        vkDestroyEvent(device, (VkEvent)mHandle, nullptr);
        break;
    case HandleType::Fence:
        vkDestroyFence(device, (VkFence)mHandle, nullptr);
        break;
    case HandleType::DeviceMemory:
        vkFreeMemory(device, (VkDeviceMemory)mHandle, nullptr);
        break;
    case HandleType::Buffer:
        vkDestroyBuffer(device, (VkBuffer)mHandle, nullptr);
        break;
    case HandleType::BufferView:
        vkDestroyBufferView(device, (VkBufferView)mHandle, nullptr);
        break;
    case HandleType::Image:
        vkDestroyImage(device, (VkImage)mHandle, nullptr);
        break;
    case HandleType::ImageView:
        vkDestroyImageView(device, (VkImageView)mHandle, nullptr);
        break;
    case HandleType::ShaderModule:
        vkDestroyShaderModule(device, (VkShaderModule)mHandle, nullptr);
        break;
    case HandleType::PipelineLayout:
        vkDestroyPipelineLayout(device, (VkPipelineLayout)mHandle, nullptr);
        break;
    case HandleType::RenderPass:
        vkDestroyRenderPass(device, (VkRenderPass)mHandle, nullptr);
        break;
    case HandleType::Pipeline:
        vkDestroyPipeline(device, (VkPipeline)mHandle, nullptr);
        break;
    case HandleType::DescriptorSetLayout:
        vkDestroyDescriptorSetLayout(device, (VkDescriptorSetLayout)mHandle, nullptr);
        break;
    case HandleType::Sampler:
        vkDestroySampler(device, (VkSampler)mHandle, nullptr);
        break;
    case HandleType::DescriptorPool:
        vkDestroyDescriptorPool(device, (VkDescriptorPool)mHandle, nullptr);
        break;
    case HandleType::Framebuffer:
        vkDestroyFramebuffer(device, (VkFramebuffer)mHandle, nullptr);
        break;
    case HandleType::CommandPool:
        vkDestroyCommandPool(device, (VkCommandPool)mHandle, nullptr);
        break;
    case HandleType::QueryPool:
        vkDestroyQueryPool(device, (VkQueryPool)mHandle, nullptr);
        break;
    default:
        UNREACHABLE();
        break;
    }

    // renderer->onDeallocateHandle(mHandleType);
}
} // namespace vk