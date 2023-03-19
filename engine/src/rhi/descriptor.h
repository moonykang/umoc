#pragma once

#include "resources.h"
#include "shader.h"

namespace rhi
{
class DescriptorSet
{
  public:
    virtual ~DescriptorSet() = default;

    virtual Result init(Context* context, DescriptorInfoList& descriptorInfoList) = 0;

    virtual Result update(rhi::Context* context, DescriptorList descriptors, std::vector<uint32_t>& offsets) = 0;

    virtual void terminate(Context* context) = 0;

    virtual void bind(Context* context, uint32_t binding) = 0;

    // virtual Result init(Context* context)
  protected:
    std::vector<Descriptor*> descriptors;
};
} // namespace rhi