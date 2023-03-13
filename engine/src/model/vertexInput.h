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
class VertexBuffer;
class IndexBuffer;
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

    Result loadIndexBuffer(platform::Context* context, std::vector<uint32_t>& indices);

    void bind(platform::Context* context);

  protected:
    rhi::VertexBuffer* vertexBuffer;
    rhi::IndexBuffer* indexBuffer;
};
} // namespace model