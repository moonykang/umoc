#include "storage.h"

#include "model/material.h"
#include "model/node.h"
#include "model/object.h"
#include "model/vertexInput.h"
#include "rhi/buffer.h"

namespace model
{
namespace predefined
{
Storage::Storage() : Object(0, 0)
{
}

Result Storage::loadVertexBuffer(platform::Context* context, double uvScale)
{
    UNREACHABLE();
    return Result::Continue;
}

Result Storage::loadVertexBuffer(platform::Context* context,
                                 std::pair<rhi::SubAllocatedBuffer*, uint32_t> storageBuffer)
{
    vertexCount = storageBuffer.second;
    return vertexInput->loadExternalVertexBuffer(context, storageBuffer.first);
}
Result Storage::loadIndexBuffer(platform::Context* context)
{
    return Result::Continue;
}
} // namespace predefined
} // namespace model
