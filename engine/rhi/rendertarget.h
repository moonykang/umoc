#pragma once

#include "resource/renderpassInfo.h"
#include <array>
#include <cstdlib>
#include <limits>
#include <optional>
#include <vector>

namespace rhi
{

/*
Attachment reference
*/
class Subpass
{
};

/*
Attachment descriptions
Subpass descriptions
Subpass dependencies *
*/

class RenderPassInfo
{
  public:
    AttachmentId registerColorAttachment(AttachmentDescription attachmentDescription);
    AttachmentId registerResolveAttachment(AttachmentDescription attachmentDescription);
    AttachmentId registerDepthStencilAttachment(AttachmentDescription attachmentDescription);

  public:
    std::vector<AttachmentDescription> ColorAttachmentDescriptions;
    std::optional<AttachmentDescription> ResolveAttachmentDescription;
    std::optional<AttachmentDescription> DepthStencilAttachmentDescription;
    std::vector<SubpassDescription> subpassDescriptions;
};

class Renderpass
{
};

} // namespace rhi