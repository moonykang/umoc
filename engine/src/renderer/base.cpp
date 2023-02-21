#include "base.h"
#include "platform/context.h"
#include "rhi/context.h"
#include "rhi/rendertarget.h"
#include "scene/scene.h"
#include "screenPass.h"

namespace renderer
{
Result BaseRenderPass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    ScreenPass screenPass;
    screenPass.render(platformContext, sceneInfo);

    try(context->present());

    return Result::Continue;
}
} // namespace renderer