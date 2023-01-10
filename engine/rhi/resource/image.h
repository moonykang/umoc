#pragma once

#include <cstdlib>

namespace rhi
{
enum class ImageLayout : uint8_t
{
    Undefined = 0,
    // Framebuffer attachment layouts are placed first, so they can fit in fewer bits in
    // PackedAttachmentOpsDesc.
    ColorAttachment = 1,
    ColorAttachmentAndFragmentShaderRead = 2,
    ColorAttachmentAndAllShadersRead = 3,
    DSAttachmentWriteAndFragmentShaderRead = 4,
    DSAttachmentWriteAndAllShadersRead = 5,
    DSAttachmentReadAndFragmentShaderRead = 6,
    DSAttachmentReadAndAllShadersRead = 7,
    DepthStencilAttachmentReadOnly = 8,
    DepthStencilAttachment = 9,
    DepthStencilResolveAttachment = 10,
    Present = 11,
    SharedPresent = 12,
    // The rest of the layouts.
    ExternalPreInitialized = 13,
    ExternalShadersReadOnly = 14,
    ExternalShadersWrite = 15,
    TransferSrc = 16,
    TransferDst = 17,
    VertexShaderReadOnly = 18,
    VertexShaderWrite = 19,
    // PreFragment == Vertex, Tessellation and Geometry stages
    PreFragmentShadersReadOnly = 20,
    PreFragmentShadersWrite = 21,
    FragmentShaderReadOnly = 22,
    FragmentShaderWrite = 23,
    ComputeShaderReadOnly = 24,
    ComputeShaderWrite = 25,
    AllGraphicsShadersReadOnly = 26,
    AllGraphicsShadersWrite = 27,

    InvalidEnum,
    EnumCount = InvalidEnum,
};
} // namespace rhi