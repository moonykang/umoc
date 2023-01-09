#include "rhi/rendertarget.h"
#include "common/util.h"

namespace rhi
{
AttachmentId RenderPassInfo::registerColorAttachment(AttachmentDescription attachmentDescription)
{
    // Color attachments must be set before DS/Resolve attachemnts are set
    ASSERT(!ResolveAttachmentDescription.has_value() && !DepthStencilAttachmentDescription.has_value());
    AttachmentId id = static_cast<AttachmentId>(ColorAttachmentDescriptions.size());
    ColorAttachmentDescriptions.push_back(attachmentDescription);

    return id;
}
AttachmentId RenderPassInfo::registerResolveAttachment(AttachmentDescription attachmentDescription)
{
    // Resolve attachments must be set before DS attachemnts are set
    ASSERT(!DepthStencilAttachmentDescription.has_value());
    AttachmentId id = static_cast<AttachmentId>(ColorAttachmentDescriptions.size());
    ResolveAttachmentDescription = attachmentDescription;

    return id;
}
AttachmentId RenderPassInfo::registerDepthStencilAttachment(AttachmentDescription attachmentDescription)
{
    AttachmentId id = static_cast<AttachmentId>(ColorAttachmentDescriptions.size());
    if (ResolveAttachmentDescription.has_value())
    {
        id++;
    }
    DepthStencilAttachmentDescription = attachmentDescription;

    return id;
}
} // namespace rhi