#include "context.h"
#include "rhi/context.h"

namespace platform
{
Context::Context() : asset(nullptr), window(nullptr)
{
}

void Context::init()
{
}

void Context::terminate()
{
}

rhi::Context* Context::getRHI()
{
    return reinterpret_cast<rhi::Context*>(this);
}
} // namespace platform