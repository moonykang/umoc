#pragma once

#include "image.h"
#include <cstdlib>
#include <optional>
#include <vector>

namespace rhi
{
class Image;

using AttachmentId = uint8_t;

enum class AttachmentLoadOp : uint8_t
{
    Discard,
    Clear,
    Load
};

enum class AttachmentStoreOp : uint8_t
{
    Discard,
    Store
};

struct AttachmentDescription
{
    Image* image;
    AttachmentLoadOp loadOp;
    AttachmentStoreOp storeOp;
    uint32_t samples;
    ImageLayout initialLayout;
    ImageLayout finalLayout;
};

struct AttachmentReference
{
    AttachmentId id;
    ImageLayout imageLayout;
};

struct SubpassDescription
{
    std::vector<AttachmentReference> colorAttachmentReference;
    std::optional<AttachmentReference> depthAttachmentReference;
    std::optional<AttachmentReference> resolveAttachmentReference;
};
} // namespace rhi