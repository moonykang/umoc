#pragma once

#include "resources.h"
#include "shader.h"

namespace rhi
{
class DescriptorSetLayout
{
  public:
    virtual ~DescriptorSetLayout() = default;

    virtual Result init(Context* context, DescriptorInfoList& descriptorInfoList) = 0;

    virtual void terminate(Context* context) = 0;
};

class DescriptorSet
{
  private:
    std::vector<Descriptor*> descriptors;
};
} // namespace rhi