#include "testScene.h"
#include "common/util.h"
#include "model/predefined/quad.h"
#include "platform/context.h"

namespace scene
{
Result TestScene::init(platform::Context* context)
{
    quad = new model::Quad();

    return Result::Continue;
}

Result TestScene::load(platform::Context* context)
{
    quad->load(context, rhi::VertexChannel::Position | rhi::VertexChannel::Color | rhi::VertexChannel::Uv |
                            rhi::VertexChannel::Normal);

    return Result::Continue;
}

Result TestScene::udpate(platform::Context* context)
{
    return Result::Continue;
}

void TestScene::terminate(platform::Context* context)
{
    TERMINATE(quad, context);
}
} // namespace scene