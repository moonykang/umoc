#include "screenPass.h"
#include "rhi/defines.h"
#include "rhi/shader.h"

namespace renderer
{
class ScreenPassVertexShader : public rhi::VertexShaderBase
{
    ScreenPassVertexShader()
        : rhi::VertexShaderBase("screen.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv)
    {
    }
};

Result ScreenPass::render(platform::Context* context)
{


    return Result::Continue;
}
} // namespace renderer