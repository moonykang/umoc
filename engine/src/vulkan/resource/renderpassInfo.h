#pragma once

#include "rhi/resources.h"
#include "vulkan/core.h"

namespace vk
{
inline VkAttachmentLoadOp convertAttachmentLoadOp(rhi::AttachmentLoadOp loadOp)
{
    switch (loadOp)
    {
    case rhi::AttachmentLoadOp::Discard:
        return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    case rhi::AttachmentLoadOp::Clear:
        return VK_ATTACHMENT_LOAD_OP_CLEAR;
    case rhi::AttachmentLoadOp::Load:
        return VK_ATTACHMENT_LOAD_OP_LOAD;
    default:
        UNREACHABLE();
        return VK_ATTACHMENT_LOAD_OP_NONE_EXT;
    }
}

inline VkAttachmentStoreOp convertAttachmentStoreOp(rhi::AttachmentStoreOp loadOp)
{
    switch (loadOp)
    {
    case rhi::AttachmentStoreOp::Discard:
        return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    case rhi::AttachmentStoreOp::Store:
        return VK_ATTACHMENT_STORE_OP_STORE;
    default:
        UNREACHABLE();
        return VK_ATTACHMENT_STORE_OP_NONE;
    }
}
} // namespace vk