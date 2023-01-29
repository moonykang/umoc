#include "rhi/context.h"
#include "vulkan/context.h"

namespace rhi
{
Context* Context::createRHIContext(rhi::List rhi)
{
    switch (rhi)
    {
    case rhi::List::Vulkan:
        return new vk::Context();
    default:
        UNREACHABLE();
        return nullptr;
    }
}
} // namespace rhi