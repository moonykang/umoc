#pragma once
#include <cstdlib>
#include <string>

namespace rhi
{
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

const inline std::string toString(Format format)
{
    switch (format)
    {
    case Format::NONE:
        return "None";
    case Format::D16_UNORM:
        return "D16_UNORM";
    case Format::D24_UNORM_S8_UINT:
        return "D24_UNORM_S8_UINT";
    case Format::D24_UNORM_X8_UINT:
        return "D24_UNORM_X8_UINT";
    case Format::D32_FLOAT:
        return "D32_FLOAT";
    case Format::D32_FLOAT_S8X24_UINT:
        return "D32_FLOAT_S8X24_UINT";
    case Format::D32_UNORM:
        return "D32_UNORM";
    case Format::S8_UINT:
        return "S8_UINT";
    case Format::A16_FLOAT:
        return "A16_FLOAT";
    case Format::A1R5G5B5_UNORM:
        return "A1R5G5B5_UNORM";
    case Format::A2R10G10B10_SINT_VERTEX:
        return "A2R10G10B10_SINT_VERTEX";
    case Format::A2R10G10B10_SNORM_VERTEX:
        return "A2R10G10B10_SNORM_VERTEX";
    case Format::A2R10G10B10_SSCALED_VERTEX:
        return "A2R10G10B10_SSCALED_VERTEX";
    case Format::A2R10G10B10_UINT_VERTEX:
        return "A2R10G10B10_UINT_VERTEX";
    case Format::A2R10G10B10_UNORM_VERTEX:
        return "A2R10G10B10_UNORM_VERTEX";
    case Format::A2R10G10B10_USCALED_VERTEX:
        return "A2R10G10B10_USCALED_VERTEX";
    case Format::A32_FLOAT:
        return "A32_FLOAT";
    case Format::A8_UNORM:
        return "A8_UNORM";
    case Format::ASTC_10x10_SRGB_BLOCK:
        return "ASTC_10x10_SRGB_BLOCK";
    case Format::ASTC_10x10_UNORM_BLOCK:
        return "ASTC_10x10_UNORM_BLOCK";
    case Format::ASTC_10x5_SRGB_BLOCK:
        return "ASTC_10x5_SRGB_BLOCK";
    case Format::ASTC_10x5_UNORM_BLOCK:
        return "ASTC_10x5_UNORM_BLOCK";
    case Format::ASTC_10x6_SRGB_BLOCK:
        return "ASTC_10x6_SRGB_BLOCK";
    case Format::ASTC_10x6_UNORM_BLOCK:
        return "ASTC_10x6_UNORM_BLOCK";
    case Format::ASTC_10x8_SRGB_BLOCK:
        return "ASTC_10x8_SRGB_BLOCK";
    case Format::ASTC_10x8_UNORM_BLOCK:
        return "ASTC_10x8_UNORM_BLOCK";
    case Format::ASTC_12x10_SRGB_BLOCK:
        return "ASTC_12x10_SRGB_BLOCK";
    case Format::ASTC_12x10_UNORM_BLOCK:
        return "ASTC_12x10_UNORM_BLOCK";
    case Format::ASTC_12x12_SRGB_BLOCK:
        return "ASTC_12x12_SRGB_BLOCK";
    case Format::ASTC_12x12_UNORM_BLOCK:
        return "ASTC_12x12_UNORM_BLOCK";
    case Format::ASTC_3x3x3_UNORM_BLOCK:
        return "ASTC_3x3x3_UNORM_BLOCK";
    case Format::ASTC_3x3x3_UNORM_SRGB_BLOCK:
        return "ASTC_3x3x3_UNORM_SRGB_BLOCK";
    case Format::ASTC_4x3x3_UNORM_BLOCK:
        return "ASTC_4x3x3_UNORM_BLOCK";
    case Format::ASTC_4x3x3_UNORM_SRGB_BLOCK:
        return "ASTC_4x3x3_UNORM_SRGB_BLOCK";
    case Format::ASTC_4x4_SRGB_BLOCK:
        return "ASTC_4x4_SRGB_BLOCK";
    case Format::ASTC_4x4_UNORM_BLOCK:
        return "NoASTC_4x4_UNORM_BLOCKne";
    case Format::ASTC_4x4x3_UNORM_BLOCK:
        return "ASTC_4x4x3_UNORM_BLOCK";
    case Format::ASTC_4x4x3_UNORM_SRGB_BLOCK:
        return "ASTC_4x4x3_UNORM_SRGB_BLOCK";
    case Format::ASTC_4x4x4_UNORM_BLOCK:
        return "ASTC_4x4x4_UNORM_BLOCK";
    case Format::ASTC_4x4x4_UNORM_SRGB_BLOCK:
        return "ASTC_4x4x4_UNORM_SRGB_BLOCK";
    case Format::ASTC_5x4_SRGB_BLOCK:
        return "ASTC_5x4_SRGB_BLOCK";
    case Format::ASTC_5x4_UNORM_BLOCK:
        return "ASTC_5x4_UNORM_BLOCK";
    case Format::ASTC_5x4x4_UNORM_BLOCK:
        return "ASTC_5x4x4_UNORM_BLOCK";
    case Format::ASTC_5x4x4_UNORM_SRGB_BLOCK:
        return "ASTC_5x4x4_UNORM_SRGB_BLOCK";
    case Format::ASTC_5x5_SRGB_BLOCK:
        return "ASTC_5x5_SRGB_BLOCK";
    case Format::ASTC_5x5_UNORM_BLOCK:
        return "ASTC_5x5_UNORM_BLOCK";
    case Format::ASTC_5x5x4_UNORM_BLOCK:
        return "ASTC_5x5x4_UNORM_BLOCK";
    case Format::ASTC_5x5x4_UNORM_SRGB_BLOCK:
        return "ASTC_5x5x4_UNORM_SRGB_BLOCK";
    case Format::ASTC_5x5x5_UNORM_BLOCK:
        return "ASTC_5x5x5_UNORM_BLOCK";
    case Format::ASTC_5x5x5_UNORM_SRGB_BLOCK:
        return "ASTC_5x5x5_UNORM_SRGB_BLOCK";
    case Format::ASTC_6x5_SRGB_BLOCK:
        return "ASTC_6x5_SRGB_BLOCK";
    case Format::ASTC_6x5_UNORM_BLOCK:
        return "ASTC_6x5_UNORM_BLOCK";
    case Format::ASTC_6x5x5_UNORM_BLOCK:
        return "ASTC_6x5x5_UNORM_BLOCK";
    case Format::ASTC_6x5x5_UNORM_SRGB_BLOCK:
        return "ASTC_6x5x5_UNORM_SRGB_BLOCK";
    case Format::ASTC_6x6_SRGB_BLOCK:
        return "ASTC_6x6_SRGB_BLOCK";
    case Format::ASTC_6x6_UNORM_BLOCK:
        return "ASTC_6x6_UNORM_BLOCK";
    case Format::ASTC_6x6x5_UNORM_BLOCK:
        return "ASTC_6x6x5_UNORM_BLOCK";
    case Format::ASTC_6x6x5_UNORM_SRGB_BLOCK:
        return "ASTC_6x6x5_UNORM_SRGB_BLOCK";
    case Format::ASTC_6x6x6_UNORM_BLOCK:
        return "ASTC_6x6x6_UNORM_BLOCK";
    case Format::ASTC_6x6x6_UNORM_SRGB_BLOCK:
        return "ASTC_6x6x6_UNORM_SRGB_BLOCK";
    case Format::ASTC_8x5_SRGB_BLOCK:
        return "ASTC_8x5_SRGB_BLOCK";
    case Format::ASTC_8x5_UNORM_BLOCK:
        return "ASTC_8x5_UNORM_BLOCK";
    case Format::ASTC_8x6_SRGB_BLOCK:
        return "ASTC_8x6_SRGB_BLOCK";
    case Format::ASTC_8x6_UNORM_BLOCK:
        return "ASTC_8x6_UNORM_BLOCK";
    case Format::ASTC_8x8_SRGB_BLOCK:
        return "ASTC_8x8_SRGB_BLOCK";
    case Format::ASTC_8x8_UNORM_BLOCK:
        return "ASTC_8x8_UNORM_BLOCK";
    case Format::B10G10R10A2_UNORM:
        return "B10G10R10A2_UNORM";
    case Format::B4G4R4A4_UNORM:
        return "B4G4R4A4_UNORM";
    case Format::B5G5R5A1_UNORM:
        return "B5G5R5A1_UNORM";
    case Format::B5G6R5_UNORM:
        return "B5G6R5_UNORM";
    case Format::B8G8R8A8_TYPELESS:
        return "B8G8R8A8_TYPELESS";
    case Format::B8G8R8A8_TYPELESS_SRGB:
        return "B8G8R8A8_TYPELESS_SRGB";
    case Format::B8G8R8A8_UNORM:
        return "B8G8R8A8_UNORM";
    case Format::B8G8R8A8_UNORM_SRGB:
        return "B8G8R8A8_UNORM_SRGB";
    case Format::B8G8R8X8_UNORM:
        return "B8G8R8X8_UNORM";
    case Format::BC1_RGBA_UNORM_BLOCK:
        return "BC1_RGBA_UNORM_BLOCK";
    case Format::BC1_RGBA_UNORM_SRGB_BLOCK:
        return "BC1_RGBA_UNORM_SRGB_BLOCK";
    case Format::BC1_RGB_UNORM_BLOCK:
        return "BC1_RGB_UNORM_BLOCK";
    case Format::BC1_RGB_UNORM_SRGB_BLOCK:
        return "BC1_RGB_UNORM_SRGB_BLOCK";
    case Format::BC2_RGBA_UNORM_BLOCK:
        return "BC2_RGBA_UNORM_BLOCK";
    case Format::BC2_RGBA_UNORM_SRGB_BLOCK:
        return "BC2_RGBA_UNORM_SRGB_BLOCK";
    case Format::BC3_RGBA_UNORM_BLOCK:
        return "BC3_RGBA_UNORM_BLOCK";
    case Format::BC3_RGBA_UNORM_SRGB_BLOCK:
        return "BC3_RGBA_UNORM_SRGB_BLOCK";
    case Format::BC4_RED_SNORM_BLOCK:
        return "BC4_RED_SNORM_BLOCK";
    case Format::BC4_RED_UNORM_BLOCK:
        return "BC4_RED_UNORM_BLOCK";
    case Format::BC5_RG_SNORM_BLOCK:
        return "BC5_RG_SNORM_BLOCK";
    case Format::BC5_RG_UNORM_BLOCK:
        return "BC5_RG_UNORM_BLOCK";
    case Format::BC6H_RGB_SFLOAT_BLOCK:
        return "BC6H_RGB_SFLOAT_BLOCK";
    case Format::BC6H_RGB_UFLOAT_BLOCK:
        return "BC6H_RGB_UFLOAT_BLOCK";
    case Format::BC7_RGBA_UNORM_BLOCK:
        return "BC7_RGBA_UNORM_BLOCK";
    case Format::BC7_RGBA_UNORM_SRGB_BLOCK:
        return "BC7_RGBA_UNORM_SRGB_BLOCK";
    case Format::EAC_R11G11_SNORM_BLOCK:
        return "EAC_R11G11_SNORM_BLOCK";
    case Format::EAC_R11G11_UNORM_BLOCK:
        return "EAC_R11G11_UNORM_BLOCK";
    case Format::EAC_R11_SNORM_BLOCK:
        return "EAC_R11_SNORM_BLOCK";
    case Format::EAC_R11_UNORM_BLOCK:
        return "EAC_R11_UNORM_BLOCK";
    case Format::ETC1_LOSSY_DECODE_R8G8B8_UNORM_BLOCK:
        return "ETC1_LOSSY_DECODE_R8G8B8_UNORM_BLOCK";
    case Format::ETC1_R8G8B8_UNORM_BLOCK:
        return "ETC1_R8G8B8_UNORM_BLOCK";
    case Format::ETC2_R8G8B8A1_SRGB_BLOCK:
        return "ETC2_R8G8B8A1_SRGB_BLOCK";
    case Format::ETC2_R8G8B8A1_UNORM_BLOCK:
        return "ETC2_R8G8B8A1_UNORM_BLOCK";
    case Format::ETC2_R8G8B8A8_SRGB_BLOCK:
        return "ETC2_R8G8B8A8_SRGB_BLOCK";
    case Format::ETC2_R8G8B8A8_UNORM_BLOCK:
        return "ETC2_R8G8B8A8_UNORM_BLOCK";
    case Format::ETC2_R8G8B8_SRGB_BLOCK:
        return "ETC2_R8G8B8_SRGB_BLOCK";
    case Format::ETC2_R8G8B8_UNORM_BLOCK:
        return "ETC2_R8G8B8_UNORM_BLOCK";
    case Format::G8_B8R8_2PLANE_420_UNORM:
        return "G8_B8R8_2PLANE_420_UNORM";
    case Format::G8_B8_R8_3PLANE_420_UNORM:
        return "G8_B8_R8_3PLANE_420_UNORM";
    case Format::L16A16_FLOAT:
        return "L16A16_FLOAT";
    case Format::L16_FLOAT:
        return "L16_FLOAT";
    case Format::L32A32_FLOAT:
        return "L32A32_FLOAT";
    case Format::L32_FLOAT:
        return "L32_FLOAT";
    case Format::L8A8_UNORM:
        return "L8A8_UNORM";
    case Format::L8_UNORM:
        return "L8_UNORM";
    case Format::PVRTC1_RGBA_2BPP_UNORM_BLOCK:
        return "PVRTC1_RGBA_2BPP_UNORM_BLOCK";
    case Format::PVRTC1_RGBA_2BPP_UNORM_SRGB_BLOCK:
        return "PVRTC1_RGBA_2BPP_UNORM_SRGB_BLOCK";
    case Format::PVRTC1_RGBA_4BPP_UNORM_BLOCK:
        return "PVRTC1_RGBA_4BPP_UNORM_BLOCK";
    case Format::PVRTC1_RGBA_4BPP_UNORM_SRGB_BLOCK:
        return "PVRTC1_RGBA_4BPP_UNORM_SRGB_BLOCK";
    case Format::PVRTC1_RGB_2BPP_UNORM_BLOCK:
        return "PVRTC1_RGB_2BPP_UNORM_BLOCK";
    case Format::PVRTC1_RGB_2BPP_UNORM_SRGB_BLOCK:
        return "PVRTC1_RGB_2BPP_UNORM_SRGB_BLOCK";
    case Format::PVRTC1_RGB_4BPP_UNORM_BLOCK:
        return "PVRTC1_RGB_4BPP_UNORM_BLOCK";
    case Format::PVRTC1_RGB_4BPP_UNORM_SRGB_BLOCK:
        return "PVRTC1_RGB_4BPP_UNORM_SRGB_BLOCK";
    case Format::R10G10B10A2_SINT:
        return "R10G10B10A2_SINT";
    case Format::R10G10B10A2_SNORM:
        return "R10G10B10A2_SNORM";
    case Format::R10G10B10A2_SSCALED:
        return "R10G10B10A2_SSCALED";
    case Format::R10G10B10A2_UINT:
        return "R10G10B10A2_UINT";
    case Format::R10G10B10A2_UNORM:
        return "R10G10B10A2_UNORM";
    case Format::R10G10B10A2_USCALED:
        return "R10G10B10A2_USCALED";
    case Format::R10G10B10X2_UNORM:
        return "R10G10B10X2_UNORM";
    case Format::R11G11B10_FLOAT:
        return "R11G11B10_FLOAT";
    case Format::R16G16B16A16_FLOAT:
        return "R16G16B16A16_FLOAT";
    case Format::R16G16B16A16_SINT:
        return "R16G16B16A16_SINT";
    case Format::R16G16B16A16_SNORM:
        return "R16G16B16A16_SNORM";
    case Format::R16G16B16A16_SSCALED:
        return "R16G16B16A16_SSCALED";
    case Format::R16G16B16A16_UINT:
        return "R16G16B16A16_UINT";
    case Format::R16G16B16A16_UNORM:
        return "R16G16B16A16_UNORM";
    case Format::R16G16B16A16_USCALED:
        return "R16G16B16A16_USCALED";
    case Format::R16G16B16_FLOAT:
        return "R16G16B16_FLOAT";
    case Format::R16G16B16_SINT:
        return "R16G16B16_SINT";
    case Format::R16G16B16_SNORM:
        return "R16G16B16_SNORM";
    case Format::R16G16B16_SSCALED:
        return "R16G16B16_SSCALED";
    case Format::R16G16B16_UINT:
        return "R16G16B16_UINT";
    case Format::R16G16B16_UNORM:
        return "R16G16B16_UNORM";
    case Format::R16G16B16_USCALED:
        return "R16G16B16_USCALED";
    case Format::R16G16_FLOAT:
        return "R16G16_FLOAT";
    case Format::R16G16_SINT:
        return "R16G16_SINT";
    case Format::R16G16_SNORM:
        return "R16G16_SNORM";
    case Format::R16G16_SSCALED:
        return "R16G16_SSCALED";
    case Format::R16G16_UINT:
        return "R16G16_UINT";
    case Format::R16G16_UNORM:
        return "R16G16_UNORM";
    case Format::R16G16_USCALED:
        return "R16G16_USCALED";
    case Format::R16_FLOAT:
        return "R16_FLOAT";
    case Format::R16_SINT:
        return "R16_SINT";
    case Format::R16_SNORM:
        return "R16_SNORM";
    case Format::R16_SSCALED:
        return "R16_SSCALED";
    case Format::R16_UINT:
        return "R16_UINT";
    case Format::R16_UNORM:
        return "R16_UNORM";
    case Format::R16_USCALED:
        return "R16_USCALED";
    case Format::R32G32B32A32_FIXED:
        return "R32G32B32A32_FIXED";
    case Format::R32G32B32A32_FLOAT:
        return "R32G32B32A32_FLOAT";
    case Format::R32G32B32A32_SINT:
        return "R32G32B32A32_SINT";
    case Format::R32G32B32A32_SNORM:
        return "R32G32B32A32_SNORM";
    case Format::R32G32B32A32_SSCALED:
        return "R32G32B32A32_SSCALED";
    case Format::R32G32B32A32_UINT:
        return "R32G32B32A32_UINT";
    case Format::R32G32B32A32_UNORM:
        return "R32G32B32A32_UNORM";
    case Format::R32G32B32A32_USCALED:
        return "R32G32B32A32_USCALED";
    case Format::R32G32B32_FIXED:
        return "R32G32B32_FIXED";
    case Format::R32G32B32_FLOAT:
        return "R32G32B32_FLOAT";
    case Format::R32G32B32_SINT:
        return "R32G32B32_SINT";
    case Format::R32G32B32_SNORM:
        return "R32G32B32_SNORM";
    case Format::R32G32B32_SSCALED:
        return "R32G32B32_SSCALED";
    case Format::R32G32B32_UINT:
        return "R32G32B32_UINT";
    case Format::R32G32B32_UNORM:
        return "R32G32B32_UNORM";
    case Format::R32G32B32_USCALED:
        return "R32G32B32_USCALED";
    case Format::R32G32_FIXED:
        return "R32G32_FIXED";
    case Format::R32G32_FLOAT:
        return "R32G32_FLOAT";
    case Format::R32G32_SINT:
        return "R32G32_SINT";
    case Format::R32G32_SNORM:
        return "R32G32_SNORM";
    case Format::R32G32_SSCALED:
        return "R32G32_SSCALED";
    case Format::R32G32_UINT:
        return "R32G32_UINT";
    case Format::R32G32_UNORM:
        return "R32G32_UNORM";
    case Format::R32G32_USCALED:
        return "R32G32_USCALED";
    case Format::R32_FIXED:
        return "R32_FIXED";
    case Format::R32_FLOAT:
        return "R32_FLOAT";
    case Format::R32_SINT:
        return "R32_SINT";
    case Format::R32_SNORM:
        return "R32_SNORM";
    case Format::R32_SSCALED:
        return "R32_SSCALED";
    case Format::R32_UINT:
        return "R32_UINT";
    case Format::R32_UNORM:
        return "R32_UNORM";
    case Format::R32_USCALED:
        return "R32_USCALED";
    case Format::R4G4B4A4_UNORM:
        return "R4G4B4A4_UNORM";
    case Format::R5G5B5A1_UNORM:
        return "R5G5B5A1_UNORM";
    case Format::R5G6B5_UNORM:
        return "R5G6B5_UNORM";
    case Format::R8G8B8A8_SINT:
        return "R8G8B8A8_SINT";
    case Format::R8G8B8A8_SNORM:
        return "R8G8B8A8_SNORM";
    case Format::R8G8B8A8_SSCALED:
        return "R8G8B8A8_SSCALED";
    case Format::R8G8B8A8_TYPELESS:
        return "R8G8B8A8_TYPELESS";
    case Format::R8G8B8A8_TYPELESS_SRGB:
        return "R8G8B8A8_TYPELESS_SRGB";
    case Format::R8G8B8A8_UINT:
        return "R8G8B8A8_UINT";
    case Format::R8G8B8A8_UNORM:
        return "R8G8B8A8_UNORM";
    case Format::R8G8B8A8_UNORM_SRGB:
        return "R8G8B8A8_UNORM_SRGB";
    case Format::R8G8B8A8_USCALED:
        return "R8G8B8A8_USCALED";
    case Format::R8G8B8_SINT:
        return "R8G8B8_SINT";
    case Format::R8G8B8_SNORM:
        return "R8G8B8_SNORM";
    case Format::R8G8B8_SSCALED:
        return "R8G8B8_SSCALED";
    case Format::R8G8B8_UINT:
        return "R8G8B8_UINT";
    case Format::R8G8B8_UNORM:
        return "R8G8B8_UNORM";
    case Format::R8G8B8_UNORM_SRGB:
        return "R8G8B8_UNORM_SRGB";
    case Format::R8G8B8_USCALED:
        return "R8G8B8_USCALED";
    case Format::R8G8_SINT:
        return "R8G8_SINT";
    case Format::R8G8_SNORM:
        return "R8G8_SNORM";
    case Format::R8G8_SSCALED:
        return "R8G8_SSCALED";
    case Format::R8G8_UINT:
        return "R8G8_UINT";
    case Format::R8G8_UNORM:
        return "R8G8_UNORM";
    case Format::R8G8_UNORM_SRGB:
        return "R8G8_UNORM_SRGB";
    case Format::R8G8_USCALED:
        return "R8G8_USCALED";
    case Format::R8_SINT:
        return "R8_SINT";
    case Format::R8_SNORM:
        return "R8_SNORM";
    case Format::R8_SSCALED:
        return "R8_SSCALED";
    case Format::R8_UINT:
        return "R8_UINT";
    case Format::R8_UNORM:
        return "R8_UNORM";
    case Format::R8_UNORM_SRGB:
        return "R8_UNORM_SRGB";
    case Format::R8_USCALED:
        return "R8_USCALED";
    case Format::R9G9B9E5_SHAREDEXP:
        return "R9G9B9E5_SHAREDEXP";
    case Format::X2R10G10B10_SINT_VERTEX:
        return "X2R10G10B10_SINT_VERTEX";
    case Format::X2R10G10B10_SNORM_VERTEX:
        return "X2R10G10B10_SNORM_VERTEX";
    case Format::X2R10G10B10_SSCALED_VERTEX:
        return "X2R10G10B10_SSCALED_VERTEX";
    case Format::X2R10G10B10_UINT_VERTEX:
        return "X2R10G10B10_UINT_VERTEX";
    case Format::X2R10G10B10_UNORM_VERTEX:
        return "X2R10G10B10_UNORM_VERTEX";
    case Format::X2R10G10B10_USCALED_VERTEX:
        return "X2R10G10B10_USCALED_VERTEX";
    }
    return "NONE";
}
} // namespace rhi