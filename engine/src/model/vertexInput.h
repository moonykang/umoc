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

    virtual Result load(platform::Context* context, rhi::VertexChannelFlags vertexChannels) = 0;

    void terminate(platform::Context* context);

    virtual void bind(rhi::Context* context) = 0;

  protected:
    rhi::VertexBuffer* vertexBuffer;
};
} // namespace model