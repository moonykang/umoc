#pragma once

#include "common/external.h"
#include <cstdint>
#include <cstdlib>

namespace rhi
{
enum class List
{
    Vulkan
};

// Format
enum class Format : uint8_t
{
    NONE,
    D16_UNORM,
    D24_UNORM_S8_UINT,
    D24_UNORM_X8_UINT,
    D32_FLOAT,
    D32_FLOAT_S8X24_UINT,
    D32_UNORM,
    S8_UINT,
    A16_FLOAT,
    A1R5G5B5_UNORM,
    A2R10G10B10_SINT_VERTEX,
    A2R10G10B10_SNORM_VERTEX,
    A2R10G10B10_SSCALED_VERTEX,
    A2R10G10B10_UINT_VERTEX,
    A2R10G10B10_UNORM_VERTEX,
    A2R10G10B10_USCALED_VERTEX,
    A32_FLOAT,
    A8_UNORM,
    ASTC_10x10_SRGB_BLOCK,
    ASTC_10x10_UNORM_BLOCK,
    ASTC_10x5_SRGB_BLOCK,
    ASTC_10x5_UNORM_BLOCK,
    ASTC_10x6_SRGB_BLOCK,
    ASTC_10x6_UNORM_BLOCK,
    ASTC_10x8_SRGB_BLOCK,
    ASTC_10x8_UNORM_BLOCK,
    ASTC_12x10_SRGB_BLOCK,
    ASTC_12x10_UNORM_BLOCK,
    ASTC_12x12_SRGB_BLOCK,
    ASTC_12x12_UNORM_BLOCK,
    ASTC_3x3x3_UNORM_BLOCK,
    ASTC_3x3x3_UNORM_SRGB_BLOCK,
    ASTC_4x3x3_UNORM_BLOCK,
    ASTC_4x3x3_UNORM_SRGB_BLOCK,
    ASTC_4x4_SRGB_BLOCK,
    ASTC_4x4_UNORM_BLOCK,
    ASTC_4x4x3_UNORM_BLOCK,
    ASTC_4x4x3_UNORM_SRGB_BLOCK,
    ASTC_4x4x4_UNORM_BLOCK,
    ASTC_4x4x4_UNORM_SRGB_BLOCK,
    ASTC_5x4_SRGB_BLOCK,
    ASTC_5x4_UNORM_BLOCK,
    ASTC_5x4x4_UNORM_BLOCK,
    ASTC_5x4x4_UNORM_SRGB_BLOCK,
    ASTC_5x5_SRGB_BLOCK,
    ASTC_5x5_UNORM_BLOCK,
    ASTC_5x5x4_UNORM_BLOCK,
    ASTC_5x5x4_UNORM_SRGB_BLOCK,
    ASTC_5x5x5_UNORM_BLOCK,
    ASTC_5x5x5_UNORM_SRGB_BLOCK,
    ASTC_6x5_SRGB_BLOCK,
    ASTC_6x5_UNORM_BLOCK,
    ASTC_6x5x5_UNORM_BLOCK,
    ASTC_6x5x5_UNORM_SRGB_BLOCK,
    ASTC_6x6_SRGB_BLOCK,
    ASTC_6x6_UNORM_BLOCK,
    ASTC_6x6x5_UNORM_BLOCK,
    ASTC_6x6x5_UNORM_SRGB_BLOCK,
    ASTC_6x6x6_UNORM_BLOCK,
    ASTC_6x6x6_UNORM_SRGB_BLOCK,
    ASTC_8x5_SRGB_BLOCK,
    ASTC_8x5_UNORM_BLOCK,
    ASTC_8x6_SRGB_BLOCK,
    ASTC_8x6_UNORM_BLOCK,
    ASTC_8x8_SRGB_BLOCK,
    ASTC_8x8_UNORM_BLOCK,
    B10G10R10A2_UNORM,
    B4G4R4A4_UNORM,
    B5G5R5A1_UNORM,
    B5G6R5_UNORM,
    B8G8R8A8_TYPELESS,
    B8G8R8A8_TYPELESS_SRGB,
    B8G8R8A8_UNORM,
    B8G8R8A8_UNORM_SRGB,
    B8G8R8X8_UNORM,
    BC1_RGBA_UNORM_BLOCK,
    BC1_RGBA_UNORM_SRGB_BLOCK,
    BC1_RGB_UNORM_BLOCK,
    BC1_RGB_UNORM_SRGB_BLOCK,
    BC2_RGBA_UNORM_BLOCK,
    BC2_RGBA_UNORM_SRGB_BLOCK,
    BC3_RGBA_UNORM_BLOCK,
    BC3_RGBA_UNORM_SRGB_BLOCK,
    BC4_RED_SNORM_BLOCK,
    BC4_RED_UNORM_BLOCK,
    BC5_RG_SNORM_BLOCK,
    BC5_RG_UNORM_BLOCK,
    BC6H_RGB_SFLOAT_BLOCK,
    BC6H_RGB_UFLOAT_BLOCK,
    BC7_RGBA_UNORM_BLOCK,
    BC7_RGBA_UNORM_SRGB_BLOCK,
    EAC_R11G11_SNORM_BLOCK,
    EAC_R11G11_UNORM_BLOCK,
    EAC_R11_SNORM_BLOCK,
    EAC_R11_UNORM_BLOCK,
    ETC1_LOSSY_DECODE_R8G8B8_UNORM_BLOCK,
    ETC1_R8G8B8_UNORM_BLOCK,
    ETC2_R8G8B8A1_SRGB_BLOCK,
    ETC2_R8G8B8A1_UNORM_BLOCK,
    ETC2_R8G8B8A8_SRGB_BLOCK,
    ETC2_R8G8B8A8_UNORM_BLOCK,
    ETC2_R8G8B8_SRGB_BLOCK,
    ETC2_R8G8B8_UNORM_BLOCK,
    G8_B8R8_2PLANE_420_UNORM,
    G8_B8_R8_3PLANE_420_UNORM,
    L16A16_FLOAT,
    L16_FLOAT,
    L32A32_FLOAT,
    L32_FLOAT,
    L8A8_UNORM,
    L8_UNORM,
    PVRTC1_RGBA_2BPP_UNORM_BLOCK,
    PVRTC1_RGBA_2BPP_UNORM_SRGB_BLOCK,
    PVRTC1_RGBA_4BPP_UNORM_BLOCK,
    PVRTC1_RGBA_4BPP_UNORM_SRGB_BLOCK,
    PVRTC1_RGB_2BPP_UNORM_BLOCK,
    PVRTC1_RGB_2BPP_UNORM_SRGB_BLOCK,
    PVRTC1_RGB_4BPP_UNORM_BLOCK,
    PVRTC1_RGB_4BPP_UNORM_SRGB_BLOCK,
    R10G10B10A2_SINT,
    R10G10B10A2_SNORM,
    R10G10B10A2_SSCALED,
    R10G10B10A2_UINT,
    R10G10B10A2_UNORM,
    R10G10B10A2_USCALED,
    R10G10B10X2_UNORM,
    R11G11B10_FLOAT,
    R16G16B16A16_FLOAT,
    R16G16B16A16_SINT,
    R16G16B16A16_SNORM,
    R16G16B16A16_SSCALED,
    R16G16B16A16_UINT,
    R16G16B16A16_UNORM,
    R16G16B16A16_USCALED,
    R16G16B16_FLOAT,
    R16G16B16_SINT,
    R16G16B16_SNORM,
    R16G16B16_SSCALED,
    R16G16B16_UINT,
    R16G16B16_UNORM,
    R16G16B16_USCALED,
    R16G16_FLOAT,
    R16G16_SINT,
    R16G16_SNORM,
    R16G16_SSCALED,
    R16G16_UINT,
    R16G16_UNORM,
    R16G16_USCALED,
    R16_FLOAT,
    R16_SINT,
    R16_SNORM,
    R16_SSCALED,
    R16_UINT,
    R16_UNORM,
    R16_USCALED,
    R32G32B32A32_FIXED,
    R32G32B32A32_FLOAT,
    R32G32B32A32_SINT,
    R32G32B32A32_SNORM,
    R32G32B32A32_SSCALED,
    R32G32B32A32_UINT,
    R32G32B32A32_UNORM,
    R32G32B32A32_USCALED,
    R32G32B32_FIXED,
    R32G32B32_FLOAT,
    R32G32B32_SINT,
    R32G32B32_SNORM,
    R32G32B32_SSCALED,
    R32G32B32_UINT,
    R32G32B32_UNORM,
    R32G32B32_USCALED,
    R32G32_FIXED,
    R32G32_FLOAT,
    R32G32_SINT,
    R32G32_SNORM,
    R32G32_SSCALED,
    R32G32_UINT,
    R32G32_UNORM,
    R32G32_USCALED,
    R32_FIXED,
    R32_FLOAT,
    R32_SINT,
    R32_SNORM,
    R32_SSCALED,
    R32_UINT,
    R32_UNORM,
    R32_USCALED,
    R4G4B4A4_UNORM,
    R5G5B5A1_UNORM,
    R5G6B5_UNORM,
    R8G8B8A8_SINT,
    R8G8B8A8_SNORM,
    R8G8B8A8_SSCALED,
    R8G8B8A8_TYPELESS,
    R8G8B8A8_TYPELESS_SRGB,
    R8G8B8A8_UINT,
    R8G8B8A8_UNORM,
    R8G8B8A8_UNORM_SRGB,
    R8G8B8A8_USCALED,
    R8G8B8_SINT,
    R8G8B8_SNORM,
    R8G8B8_SSCALED,
    R8G8B8_UINT,
    R8G8B8_UNORM,
    R8G8B8_UNORM_SRGB,
    R8G8B8_USCALED,
    R8G8_SINT,
    R8G8_SNORM,
    R8G8_SSCALED,
    R8G8_UINT,
    R8G8_UNORM,
    R8G8_UNORM_SRGB,
    R8G8_USCALED,
    R8_SINT,
    R8_SNORM,
    R8_SSCALED,
    R8_UINT,
    R8_UNORM,
    R8_UNORM_SRGB,
    R8_USCALED,
    R9G9B9E5_SHAREDEXP,
    X2R10G10B10_SINT_VERTEX,
    X2R10G10B10_SNORM_VERTEX,
    X2R10G10B10_SSCALED_VERTEX,
    X2R10G10B10_UINT_VERTEX,
    X2R10G10B10_UNORM_VERTEX,
    X2R10G10B10_USCALED_VERTEX
};
const uint32_t kNumFormats = 256;

// Image layout
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

// Attachment
using AttachmentId = uint8_t;

enum
{
    MaxSimultaneousRenderTargets = 8,
    MaxSimultaneousRenderTargets_NumBits = 3,
};

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

enum VertexChannel
{
    Position = 1,
    Normal = 2,
    Uv = 4,
    Color = 8,
    Tangent = 16,
    Bitangent = 32,
    Joint0 = 64,
    Weight0 = 128
};
typedef uint32_t VertexChannelFlags;

class Vertex
{
  public:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec4 color;
    glm::vec4 joint0;
    glm::vec4 weight0;
    glm::vec4 tangent;
};

enum class ShaderStage
{
    Vertex,
    Pixel
};

using MemoryPropertyFlags = uint32_t;
class MemoryProperty
{
  public:
    enum Value : uint32_t
    {
        DEVICE_LOCAL = 0x00000001,
        HOST_VISIBLE = 0x00000002,
        HOST_COHERENT = 0x00000004,
        HOST_CACHED = 0x00000008,
        LAZILY_ALLOCATED = 0x00000010,
        PROTECTED = 0x00000020,
        DEVICE_COHERENT = 0x00000040,
        DEVICE_UNCACHED = 0x00000080,
    };

    MemoryProperty() = default;

    constexpr MemoryProperty(Value value) : value(value)
    {
    }

    constexpr MemoryPropertyFlags operator|(MemoryProperty& other) const
    {
        return value | other.value;
    }

  private:
    Value value;
};

using BufferUsageFlags = uint32_t;
class BufferUsage
{
  public:
    enum Value : uint32_t
    {
        TRANSFER_SRC = 0x00000001,
        TRANSFER_DST = 0x00000002,
        UNIFORM_TEXEL_BUFFER = 0x00000004,
        STORAGE_TEXEL_BUFFER = 0x00000008,
        UNIFORM_BUFFER = 0x00000010,
        STORAGE_BUFFER = 0x00000020,
        INDEX_BUFFER = 0x00000040,
        VERTEX_BUFFER = 0x00000080,
        INDIRECT_BUFFER = 0x00000100,
        SHADER_DEVICE_ADDRESS = 0x00020000,
        VIDEO_DECODE_SRC = 0x00002000,
        VIDEO_DECODE_DST = 0x00004000,
        TRANSFORM_FEEDBACK_BUFFER = 0x00000800,
        TRANSFORM_FEEDBACK_COUNTER_BUFFER = 0x00001000,
        CONDITIONAL_RENDERING = 0x00000200,
        ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY = 0x00080000,
        ACCELERATION_STRUCTURE_STORAGE = 0x00100000,
        SHADER_BINDING_TABLE = 0x00000400,
        VIDEO_ENCODE_DST = 0x00008000,
        VIDEO_ENCODE_SRC = 0x00010000,
        MICROMAP_BUILD_INPUT_READ_ONLY = 0x00800000,
        MICROMAP_STORAGE = 0x01000000,
    };

    BufferUsage() = default;

    constexpr BufferUsage(Value value) : value(value)
    {
    }

    constexpr BufferUsageFlags operator|(BufferUsage& other) const
    {
        return value | other.value;
    }

  private:
    Value value;
};
} // namespace rhi