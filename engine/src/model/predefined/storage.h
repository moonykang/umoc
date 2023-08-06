#pragma once
#include "object.h"

namespace model
{
namespace predefined
{
class Storage : public Object
{
  public:
    Storage();

    Result loadVertexBuffer(platform::Context* context, double uvScale) override;

    Result loadVertexBuffer(platform::Context* context,
                            std::pair<rhi::SubAllocatedBuffer*, uint32_t> storageBuffer) override;

    Result loadIndexBuffer(platform::Context* context) override;
};
} // namespace predefined
} // namespace model