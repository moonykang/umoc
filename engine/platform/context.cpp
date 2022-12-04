#include "context.h"
#include "rhi/context.h"

namespace platform
{
Context::Context() : asset(nullptr), window(nullptr)
{
}

Result Context::init()
{
    try(initRHI());

    return Result::Continue;
}

void Context::terminate()
{
    terminateRHI();
}

rhi::Context* Context::getRHI()
{
    return reinterpret_cast<rhi::Context*>(this);
}
} // namespace platform