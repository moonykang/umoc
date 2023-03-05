#include "testScene.h"
#include "common/util.h"
#include "model/instance.h"
#include "model/predefined/quad.h"
#include "platform/context.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"
#include "scene/view.h"

namespace scene
{
Result TestScene::postInit(platform::Context* context)
{
    quad = new model::Quad();
    instance = new model::Instance();

    // float time = 0;
    // ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    return Result::Continue;
}

Result TestScene::load(platform::Context* context)
{
    try(quad->load(context, rhi::VertexChannel::Position | rhi::VertexChannel::Color | rhi::VertexChannel::Uv |
                                rhi::VertexChannel::Normal));

    try(instance->init(context, quad));
    try(instance->updateUniformBuffer(context));

    view->setView(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    view->setPerspective(45.0f, 1, 0.1f, 10.f);

    try(view->updateUniformBuffer(context));
    return Result::Continue;
}

Result TestScene::udpate(platform::Context* context)
{
    return Result::Continue;
}

void TestScene::preTerminate(platform::Context* context)
{
    TERMINATE(instance, context);
    TERMINATE(quad, context);
}

void TestScene::postTerminate(platform::Context* context)
{
}
} // namespace scene