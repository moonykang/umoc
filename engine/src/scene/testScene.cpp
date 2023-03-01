#include "testScene.h"
#include "common/util.h"
#include "model/predefined/quad.h"
#include "platform/context.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"

namespace scene
{
struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
} ubo;

Result TestScene::init(platform::Context* context)
{
    quad = new model::Quad();

    float time = 0;
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), 1024 / (float)1024, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;
    uniformBuffer = reinterpret_cast<rhi::UniformBuffer*>(context->getRHI()->getUniformScratchBuffer()->subAllocate(
        context->getRHI(), sizeof(UniformBufferObject), &ubo));

    descriptorSet = context->getRHI()->allocateDescriptorSet();

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
    TERMINATE(descriptorSet, context->getRHI());
    TERMINATE(quad, context);
}
} // namespace scene