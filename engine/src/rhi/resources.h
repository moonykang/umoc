#pragma once

#include "defines.h"
#include <optional>
#include <string>
#include <vector>

namespace rhi
{
// Format
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

// Attachment
class Image;

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

// Pipeline
enum class PrimitiveTopology : uint8_t
{
    POINT_LIST = 0,
    LINE_LIST = 1,
    LINE_STRIP = 2,
    TRIANGLE_LIST = 3,
    TRIANGLE_STRIP = 4,
    TRIANGLE_FAN = 5,
    LINE_LIST_WITH_ADJACENCY = 6,
    LINE_STRIP_WITH_ADJACENCY = 7,
    TRIANGLE_LIST_WITH_ADJACENCY = 8,
    TRIANGLE_STRIP_WITH_ADJACENCY = 9,
    PATCH_LIST = 10
};

class AssemblyState
{
  public:
    AssemblyState() : primitiveTopology(PrimitiveTopology::TRIANGLE_LIST), primitiveRestartEnable(false)
    {
    }

  public:
    PrimitiveTopology primitiveTopology;
    bool primitiveRestartEnable;
};

enum class PolygonMode : uint8_t
{
    FILL = 0,
    LINE = 1,
    POINT = 2
};

enum class CullMode : uint8_t
{
    NONE = 0,
    FRONT_BIT = 1,
    BACK_BIT = 2,
    FRONT_AND_BACK = 3,
};

enum class FrontFace : bool
{
    COUNTER_CLOCKWISE = 0,
    CLOCKWISE = 1,
};

class RasterizationState
{
  public:
    RasterizationState()
        : depthBiasConstantFactor(0.f), depthBiasClamp(0.f), depthBiasSlopeFactor(0.f), lineWidth(1.f),
          polygonMode(PolygonMode::FILL), cullMode(CullMode::BACK_BIT), depthClampEnable(false),
          rasterizerDiscardEnable(false), depthBiasEnable(false), frontFace(FrontFace::COUNTER_CLOCKWISE)
    {
    }

  public:
    float depthBiasConstantFactor;
    float depthBiasClamp;
    float depthBiasSlopeFactor;
    float lineWidth;
    PolygonMode polygonMode;
    CullMode cullMode;
    bool depthClampEnable;
    bool rasterizerDiscardEnable;
    bool depthBiasEnable;
    FrontFace frontFace;
};

class TessellationState
{
  public:
    TessellationState() : patchControlPoints(0)
    {
    }

  public:
    uint32_t patchControlPoints;
};

enum class SampleCount : uint8_t
{
    Sample_1 = 0x00000001,
    Sample_2 = 0x00000002,
    Sample_4 = 0x00000004,
    Sample_8 = 0x00000008,
    Sample_16 = 0x00000010,
    Sample_32 = 0x00000020,
    Sample_64 = 0x00000040,
};

class MultisampleState
{
  public:
    MultisampleState()
        : minSampleShading(0.f), sampleCount(SampleCount::Sample_1), sampleShadingEnable(false),
          alphaToCoverageEnable(false), alphaToOneEnable(false)
    {
    }

  public:
    float minSampleShading;
    SampleCount sampleCount;
    bool sampleShadingEnable;
    bool alphaToCoverageEnable;
    bool alphaToOneEnable;
};

enum class StencilOp : uint8_t
{
    KEEP = 0,
    ZERO = 1,
    REPLACE = 2,
    INCREMENT_AND_CLAMP = 3,
    DECREMENT_AND_CLAMP = 4,
    INVERT = 5,
    INCREMENT_AND_WRAP = 6,
    DECREMENT_AND_WRAP = 7
};

enum class CompareOp : uint8_t
{
    NEVER = 0,
    LESS = 1,
    EQUAL = 2,
    LESS_OR_EQUAL = 3,
    GREATER = 4,
    NOT_EQUAL = 5,
    GREATER_OR_EQUAL = 6,
    ALWAYS = 7
};

class StencilOpState
{
  public:
    StencilOpState()
        : failOp(StencilOp::KEEP), passOp(StencilOp::KEEP), depthFailOp(StencilOp::KEEP), compareOp(CompareOp::NEVER),
          compareMask(0), writeMask(0), reference(0)
    {
    }

  public:
    StencilOp failOp;
    StencilOp passOp;
    StencilOp depthFailOp;
    CompareOp compareOp;
    uint32_t compareMask;
    uint32_t writeMask;
    uint32_t reference;
};

class DepthStencilState
{
  public:
    DepthStencilState()
        : front(), back(), minDepthBounds(0.f), maxDepthBounds(0.f), depthCompareOp(CompareOp::LESS_OR_EQUAL),
          depthTestEnable(false), depthWriteEnable(false), depthBoundsTestEnable(false), stencilTestEnable(false)
    {
    }

  public:
    StencilOpState front;
    StencilOpState back;
    float minDepthBounds;
    float maxDepthBounds;
    CompareOp depthCompareOp;
    bool depthTestEnable;
    bool depthWriteEnable;
    bool depthBoundsTestEnable;
    bool stencilTestEnable;
};

enum class BlendOp : uint8_t
{
    ADD = 0,
    SUBTRACT = 1,
    REVERSE_SUBTRACT = 2,
    MIN = 3,
    MAX = 4,
};

enum class BlendFactor : uint8_t
{
    ZERO = 0,
    ONE = 1,
    SRC_COLOR = 2,
    ONE_MINUS_SRC_COLOR = 3,
    DST_COLOR = 4,
    ONE_MINUS_DST_COLOR = 5,
    SRC_ALPHA = 6,
    ONE_MINUS_SRC_ALPHA = 7,
    DST_ALPHA = 8,
    ONE_MINUS_DST_ALPHA = 9,
    CONSTANT_COLOR = 10,
    ONE_MINUS_CONSTANT_COLOR = 11,
    CONSTANT_ALPHA = 12,
    ONE_MINUS_CONSTANT_ALPHA = 13,
    SRC_ALPHA_SATURATE = 14,
    SRC1_COLOR = 15,
    ONE_MINUS_SRC1_COLOR = 16,
    SRC1_ALPHA = 17,
    ONE_MINUS_SRC1_ALPHA = 18
};

class ColorComponent
{
  public:
    enum Value : uint8_t
    {
        R = 0x00000001,
        G = 0x00000002,
        B = 0x00000004,
        A = 0x00000008
    };

    ColorComponent() = default;

    constexpr ColorComponent(Value value) : value(value)
    {
    }

    constexpr uint8_t operator|(ColorComponent& colorComponent) const
    {
        return value | colorComponent.value;
    }

  private:
    Value value;
};
using ColorComponentFlags = uint8_t;

class ColorBlendAttachmentState
{
  public:
    ColorBlendAttachmentState()
        : srcColorBlendFactor(BlendFactor::ZERO), dstColorBlendFactor(BlendFactor::ZERO),
          srcAlphaBlendFactor(BlendFactor::ZERO), dstAlphaBlendFactor(BlendFactor::ZERO), colorBlendOp(BlendOp::ADD),
          alphaBlendOp(BlendOp::ADD),
          colorWriteMask(ColorComponent::A | ColorComponent::B | ColorComponent::G | ColorComponent::R),
          blendEnable(false)
    {
    }

  public:
    BlendFactor srcColorBlendFactor;
    BlendFactor dstColorBlendFactor;
    BlendFactor srcAlphaBlendFactor;
    BlendFactor dstAlphaBlendFactor;
    BlendOp colorBlendOp;
    BlendOp alphaBlendOp;
    ColorComponentFlags colorWriteMask;
    bool blendEnable;
};

enum class LogicOp : uint8_t
{
    CLEAR = 0,
    AND = 1,
    REVERSE = 2,
    COPY = 3,
    AND_INVERTED = 4,
    NO_OP = 5,
    XOR = 6,
    OR = 7,
    NOR = 8,
    EQUIVALENT = 9,
    INVERT = 10,
    OR_REVERSE = 11,
    COPY_INVERTED = 12,
    OR_INVERTED = 13,
    NAND = 14,
    SET = 15
};

class ColorBlendState
{
  public:
    ColorBlendState() : attachmentCount(0), blendConstants{}, logicOp(LogicOp::COPY), logicOpEnable(false)
    {
    }

  public:
    ColorBlendAttachmentState colorBlendAttachmentStates[MaxSimultaneousRenderTargets];
    uint32_t attachmentCount;
    float blendConstants[4];
    LogicOp logicOp;
    bool logicOpEnable;
};

class ShaderBase;
class VertexShaderBase;
class PixelShaderBase;
class ShaderContainer;

constexpr size_t PipelineStateHashSize = sizeof(AssemblyState) + sizeof(RasterizationState) +
                                         sizeof(TessellationState) + sizeof(MultisampleState) +
                                         sizeof(DepthStencilState) + sizeof(ColorBlendState);
class GraphicsPipelineState
{
  public:
    GraphicsPipelineState() : shaderContainer(nullptr)
    {
    }

    size_t getHash();

  public:
    AssemblyState assemblyState;
    RasterizationState rasterizationState;
    TessellationState tessellationState;
    MultisampleState multisampleState;
    DepthStencilState depthStencilState;
    ColorBlendState colorBlendState;
    uint8_t padding1 = 0;
    uint8_t padding2 = 0;

    ShaderContainer* shaderContainer;
    /*
    - shader
    - vertex input
    * pipeline layout (push constants) <> descriptorset layout
    */
};

class DescriptorInfo
{
  public:
    DescriptorInfo(uint32_t binding, ShaderStageFlags shaderStage, DescriptorType type)
        : binding(binding), shaderStage(shaderStage), type(type)
    {
    }

    ~DescriptorInfo() = default;

    uint32_t getBinding()
    {
        return binding;
    }

    ShaderStageFlags getShaderStage()
    {
        return shaderStage;
    }

    DescriptorType getType()
    {
        return type;
    }

  private:
    uint32_t binding;
    ShaderStageFlags shaderStage;
    DescriptorType type;
};
using DescriptorInfoList = std::vector<DescriptorInfo>;
using DescriptorInfoListSet = std::vector<DescriptorInfoList>;

class Descriptor
{
  public:
    Descriptor(DescriptorType type) : type(type)
    {
    }

    virtual ~Descriptor() = default;

    DescriptorType getType()
    {
        return type;
    }

  protected:
    DescriptorType type;
};
using DescriptorList = std::vector<std::pair<DescriptorInfo, Descriptor*>>;
using DescriptorListSet = std::vector<DescriptorList>;
} // namespace rhi