#pragma once

#include "rhi/context.h"
#include "vulkan/wrapper.h"

namespace vk
{
class Context : public rhi::Context
{
  public:
    void initRHI() override;

    void initInstance();

  private:
    handle::Instance instance;
};
} // namespace vk