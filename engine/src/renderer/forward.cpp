#include "forward.h"
#include "rhi/shader.h"

class ForwardVertexShader : public rhi::VertexShaderBase
{
  public:
    ForwardVertexShader()
        : rhi::VertexShaderBase("triangle.vert.spv",
                                rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal)
    {
    }
};

class ForwardFragmentShader : public rhi::PixelShaderBase
{
  public:
    ForwardFragmentShader() : rhi::PixelShaderBase("triangle.frag.spv")
    {
    }
};

class ForwardShaderParameters : public rhi::ShaderParameters
{
  public:
    ForwardShaderParameters()
        : ShaderParameters(), globalDescriptor(nullptr), localDescriptor(nullptr), materialDescriptor(nullptr)
    {
    }

    std::vector<rhi::DescriptorSet*> getDescriptorSets() override
    {
        return {globalDescriptor, localDescriptor, materialDescriptor};
    }
    rhi::DescriptorSet* globalDescriptor;
    rhi::DescriptorSet* localDescriptor;
    rhi::DescriptorSet* materialDescriptor;
};

ForwardVertexShader forwardVertexShader;
ForwardFragmentShader forwardPixelShader;

namespace renderer
{
Result Forward::init(platform::Context* context)
{
    return Result::Continue;
}

void Forward::terminate(platform::Context* context)
{
}

Result Forward::render(platform::Context* context, scene::SceneInfo* sceneInfo)
{
    return Result::Continue;
}
} // namespace renderer