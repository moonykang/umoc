#include "base.h"
#include "platform/context.h"
#include "rhi/context.h"

namespace renderer
{
Result Base::init(platform::Context* context)
{

    return Result::Continue;
}

Result Base::render(platform::Context* context)
{
    rhi::Context* rhiContext = reinterpret_cast<rhi::Context*>(context);

    try(rhiContext->present());

    return Result::Continue;
}
} // namespace renderer