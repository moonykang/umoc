#include "base.h"
#include "platform/context.h"

namespace renderer
{
Result Base::init(platform::Context* context)
{

    return Result::Continue;
}

Result Base::render(platform::Context* context)
{
    LOGD("Hello!!!!");

    return Result::Continue;
}
} // namespace renderer