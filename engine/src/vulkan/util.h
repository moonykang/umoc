#pragma once

#include "common/util.h"
#include <vector>

#define DEBUG_COMMAND_BUFFER 1

#if DEBUG_COMMAND_BUFFER
#define CAPTURE_COMMAND(msg, ...) LOGD(msg, __VA_ARGS__)
#else
#define CAPTURE_COMMAND(msg, ...)
#endif

namespace vk
{
class Context;

#define VK_HANDLE_TYPES_X(FUNC)                                                                                        \
    FUNC(Allocation)                                                                                                   \
    FUNC(Allocator)                                                                                                    \
    FUNC(Buffer)                                                                                                       \
    FUNC(BufferBlock)                                                                                                  \
    FUNC(BufferView)                                                                                                   \
    FUNC(CommandPool)                                                                                                  \
    FUNC(DescriptorPool)                                                                                               \
    FUNC(DescriptorSetLayout)                                                                                          \
    FUNC(DeviceMemory)                                                                                                 \
    FUNC(Event)                                                                                                        \
    FUNC(Fence)                                                                                                        \
    FUNC(Framebuffer)                                                                                                  \
    FUNC(Image)                                                                                                        \
    FUNC(ImageView)                                                                                                    \
    FUNC(Pipeline)                                                                                                     \
    FUNC(PipelineCache)                                                                                                \
    FUNC(PipelineLayout)                                                                                               \
    FUNC(QueryPool)                                                                                                    \
    FUNC(RenderPass)                                                                                                   \
    FUNC(Sampler)                                                                                                      \
    FUNC(SamplerYcbcrConversion)                                                                                       \
    FUNC(Semaphore)                                                                                                    \
    FUNC(ShaderModule)

#define COMMA_SEP_FUNC(TYPE) TYPE,

enum class HandleType
{
    Invalid,
    CommandBuffer,
    VK_HANDLE_TYPES_X(COMMA_SEP_FUNC) EnumCount
};

VK_DEFINE_NON_DISPATCHABLE_HANDLE(GarbageHandle);
class GarbageObject
{
  public:
    GarbageObject();
    GarbageObject(HandleType handleType, GarbageHandle handle);
    GarbageObject(GarbageObject&& other);
    GarbageObject& operator=(GarbageObject&& rhs);
    bool valid() const;

    void terminate(Context* context);

    GarbageHandle getHandle()
    {
        return mHandle;
    }

  private:
    // typedef struct GarbageHandle_T* GarbageHandle;

    HandleType mHandleType;
    GarbageHandle mHandle;
};

// A list of garbage objects. Has no object lifetime information.
using GarbageList = std::vector<GarbageObject>;

} // namespace vk