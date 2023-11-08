#include "constantBlock.h"
#include "rhi/context.h"

namespace model
{

const bool ConstantBlock::valid()
{
    return !memoryBuffer.empty();
}

void ConstantBlock::udpate(rhi::ShaderStageFlags shaderStage, size_t size, const void* data)
{
    this->shaderStage = shaderStage;
    memoryBuffer.set(size, data);
}

void ConstantBlock::push(rhi::Context* context)
{
    context->pushConstant(shaderStage, memoryBuffer.size(), memoryBuffer.data());
}

size_t ConstantBlock::getSize()
{
    return memoryBuffer.size();
}

rhi::ShaderStageFlags ConstantBlock::getShaderStage()
{
    return shaderStage;
}
} // namespace model