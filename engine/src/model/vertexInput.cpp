#include "vertexInput.h"
#include "common/util.h"
#include "platform/context.h"
#include "rhi/buffer.h"
#include "rhi/context.h"

namespace model
{
VertexInput::VertexInput() : vertexBuffer(nullptr)
{
}

void VertexInput::terminate(platform::Context* context)
{
    // TERMINATE(vertexBuffer, context->getRHI());
}

} // namespace model