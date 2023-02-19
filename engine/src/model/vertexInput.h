#pragma once
#include "common/util.h"
#include "rhi/defines.h"

namespace platform
{
class Context;
}

namespace model
{
class VertexInput
{
  public:
    virtual Result load(platform::Context* context, rhi::VertexChannelFlags vertexChannels) = 0;

  protected:
};
} // namespace model