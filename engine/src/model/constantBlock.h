#pragma once

#include "common/memorybuffer.h"
#include "rhi/defines.h"

namespace rhi
{
class Context;
}

namespace model
{
class ConstantBlock
{
  public:
    const bool valid();

    void udpate(rhi::ShaderStageFlags shaderStage, size_t size, const void* data);

    void push(rhi::Context* context);

    size_t getSize();

    rhi::ShaderStageFlags getShaderStage();

  private:
    util::MemoryBuffer memoryBuffer;
    rhi::ShaderStageFlags shaderStage;
};
} // namespace model