#include "base.h"
#include "platform/context.h"
#include "rhi/context.h"
#include "rhi/rendertarget.h"

namespace renderer
{
Result Base::init(platform::Context* context)
{

    return Result::Continue;
}

/*
struct AttachmentDescription
{
    Image* image;
    AttachmentLoadOp loadOp;
    AttachmentStoreOp storeOp;
    uint32_t samples;
    ImageLayout initialLayout;
    ImageLayout finalLayout;
};
*/
Result Base::render(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    rhi::RenderPassInfo renderpassInfo;
    rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
        {context->getCurrentSurfaceImage(), rhi::AttachmentLoadOp::Clear, rhi::AttachmentStoreOp::Store, 1,
         rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::Present});

    auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
    subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});

    try(context->beginRenderpass(renderpassInfo));

    try(context->present());

    return Result::Continue;
}
} // namespace renderer