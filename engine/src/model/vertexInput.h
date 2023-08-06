#pragma once
#include "common/util.h"
#include "rhi/defines.h"

namespace platform
{
class Context;
}

namespace rhi
{
class Context;
class SubAllocatedBuffer;
} // namespace rhi

namespace model
{
class VertexInput
{
  public:
    VertexInput();

    virtual ~VertexInput() = default;

    void terminate(platform::Context* context);

    Result loadVertexBuffer(platform::Context* context, std::vector<rhi::Vertex>& vertices);

    Result loadExternalVertexBuffer(platform::Context* context, rhi::SubAllocatedBuffer* externalBuffer);

    Result loadIndexBuffer(platform::Context* context, std::vector<uint32_t>& indices);

    void bind(platform::Context* context);

  protected:
    rhi::SubAllocatedBuffer* vertexBuffer;
    rhi::SubAllocatedBuffer* indexBuffer;
};
} // namespace model