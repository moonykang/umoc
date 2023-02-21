#pragma once

#include "../object.h"
#include "../vertexInput.h"
#include "common/util.h"

namespace model
{
class Quad : public VertexInput
{
  public:
    Result load(platform::Context* context, rhi::VertexChannelFlags vertexChannels) override;

    void bind(rhi::Context* context) override;
};
} // namespace model
