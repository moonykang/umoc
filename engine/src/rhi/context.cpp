#include "rhi/context.h"
#include "common/util.h"
#include "rhi/shader.h"
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

Context::Context() : platform::Context()
{
}

Result Context::initRHI(platform::Window* window)
{
    try(initRHIImplementation(window));

    return Result::Continue;
}

void Context::terminateRHI()
{
    terminateRHIImplementation();
}
} // namespace rhi