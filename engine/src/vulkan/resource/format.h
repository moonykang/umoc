#pragma once
#include "rhi/resources.h"
#include "vulkan/core.h"
#include <map>

namespace vk
{

struct Format
{
    VkFormat format;
    VkImageAspectFlags aspects;
};

static std::map<rhi::Format, Format> kFormatMap = {
    // Color
    {rhi::Format::A1R5G5B5_UNORM, {VK_FORMAT_A1R5G5B5_UNORM_PACK16, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_10x10_SRGB_BLOCK, {VK_FORMAT_ASTC_10x10_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_10x10_UNORM_BLOCK, {VK_FORMAT_ASTC_10x10_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_10x5_SRGB_BLOCK, {VK_FORMAT_ASTC_10x5_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_10x5_UNORM_BLOCK, {VK_FORMAT_ASTC_10x5_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_10x6_SRGB_BLOCK, {VK_FORMAT_ASTC_10x6_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_10x6_UNORM_BLOCK, {VK_FORMAT_ASTC_10x6_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_10x8_SRGB_BLOCK, {VK_FORMAT_ASTC_10x8_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_10x8_UNORM_BLOCK, {VK_FORMAT_ASTC_10x8_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_12x10_SRGB_BLOCK, {VK_FORMAT_ASTC_12x10_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_12x10_UNORM_BLOCK, {VK_FORMAT_ASTC_12x10_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_12x12_SRGB_BLOCK, {VK_FORMAT_ASTC_12x12_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_12x12_UNORM_BLOCK, {VK_FORMAT_ASTC_12x12_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_4x4_SRGB_BLOCK, {VK_FORMAT_ASTC_4x4_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_4x4_UNORM_BLOCK, {VK_FORMAT_ASTC_4x4_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_5x4_SRGB_BLOCK, {VK_FORMAT_ASTC_5x4_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_5x4_UNORM_BLOCK, {VK_FORMAT_ASTC_5x4_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_5x5_SRGB_BLOCK, {VK_FORMAT_ASTC_5x5_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_5x5_UNORM_BLOCK, {VK_FORMAT_ASTC_5x5_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_6x5_SRGB_BLOCK, {VK_FORMAT_ASTC_6x5_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_6x5_UNORM_BLOCK, {VK_FORMAT_ASTC_6x5_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_6x6_SRGB_BLOCK, {VK_FORMAT_ASTC_6x6_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_6x6_UNORM_BLOCK, {VK_FORMAT_ASTC_6x6_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_8x5_SRGB_BLOCK, {VK_FORMAT_ASTC_8x5_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_8x5_UNORM_BLOCK, {VK_FORMAT_ASTC_8x5_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_8x6_SRGB_BLOCK, {VK_FORMAT_ASTC_8x6_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_8x6_UNORM_BLOCK, {VK_FORMAT_ASTC_8x6_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_8x8_SRGB_BLOCK, {VK_FORMAT_ASTC_8x8_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ASTC_8x8_UNORM_BLOCK, {VK_FORMAT_ASTC_8x8_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::B10G10R10A2_UNORM, {VK_FORMAT_A2R10G10B10_UNORM_PACK32, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::B4G4R4A4_UNORM, {VK_FORMAT_B4G4R4A4_UNORM_PACK16, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::B5G5R5A1_UNORM, {VK_FORMAT_B5G5R5A1_UNORM_PACK16, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::B5G6R5_UNORM, {VK_FORMAT_B5G6R5_UNORM_PACK16, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::B8G8R8A8_UNORM, {VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::B8G8R8A8_UNORM_SRGB, {VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::BC1_RGBA_UNORM_BLOCK, {VK_FORMAT_BC1_RGBA_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::BC1_RGBA_UNORM_SRGB_BLOCK, {VK_FORMAT_BC1_RGBA_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::BC1_RGB_UNORM_BLOCK, {VK_FORMAT_BC1_RGB_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::BC1_RGB_UNORM_SRGB_BLOCK, {VK_FORMAT_BC1_RGB_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::BC2_RGBA_UNORM_BLOCK, {VK_FORMAT_BC2_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::BC2_RGBA_UNORM_SRGB_BLOCK, {VK_FORMAT_BC2_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::BC3_RGBA_UNORM_BLOCK, {VK_FORMAT_BC3_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::BC3_RGBA_UNORM_SRGB_BLOCK, {VK_FORMAT_BC3_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::BC4_RED_SNORM_BLOCK, {VK_FORMAT_BC4_SNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::BC4_RED_UNORM_BLOCK, {VK_FORMAT_BC4_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::BC5_RG_SNORM_BLOCK, {VK_FORMAT_BC5_SNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::BC5_RG_UNORM_BLOCK, {VK_FORMAT_BC5_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::BC6H_RGB_SFLOAT_BLOCK, {VK_FORMAT_BC6H_SFLOAT_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::BC6H_RGB_UFLOAT_BLOCK, {VK_FORMAT_BC6H_UFLOAT_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::BC7_RGBA_UNORM_BLOCK, {VK_FORMAT_BC7_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::BC7_RGBA_UNORM_SRGB_BLOCK, {VK_FORMAT_BC7_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::EAC_R11G11_SNORM_BLOCK, {VK_FORMAT_EAC_R11G11_SNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::EAC_R11G11_UNORM_BLOCK, {VK_FORMAT_EAC_R11G11_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::EAC_R11_SNORM_BLOCK, {VK_FORMAT_EAC_R11_SNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::EAC_R11_UNORM_BLOCK, {VK_FORMAT_EAC_R11_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ETC2_R8G8B8A1_SRGB_BLOCK, {VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ETC2_R8G8B8A1_UNORM_BLOCK, {VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ETC2_R8G8B8A8_SRGB_BLOCK, {VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ETC2_R8G8B8A8_UNORM_BLOCK, {VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ETC2_R8G8B8_SRGB_BLOCK, {VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::ETC2_R8G8B8_UNORM_BLOCK, {VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::G8_B8R8_2PLANE_420_UNORM, {VK_FORMAT_G8_B8R8_2PLANE_420_UNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::G8_B8_R8_3PLANE_420_UNORM, {VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::NONE, {VK_FORMAT_UNDEFINED, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R10G10B10A2_SINT, {VK_FORMAT_A2B10G10R10_SINT_PACK32, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R10G10B10A2_SNORM, {VK_FORMAT_A2B10G10R10_SNORM_PACK32, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R10G10B10A2_SSCALED, {VK_FORMAT_A2B10G10R10_SSCALED_PACK32, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R10G10B10A2_UINT, {VK_FORMAT_A2B10G10R10_UINT_PACK32, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R10G10B10A2_UNORM, {VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R10G10B10A2_USCALED, {VK_FORMAT_A2B10G10R10_USCALED_PACK32, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R11G11B10_FLOAT, {VK_FORMAT_B10G11R11_UFLOAT_PACK32, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16B16A16_FLOAT, {VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16B16A16_SINT, {VK_FORMAT_R16G16B16A16_SINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16B16A16_SNORM, {VK_FORMAT_R16G16B16A16_SNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16B16A16_SSCALED, {VK_FORMAT_R16G16B16A16_SSCALED, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16B16A16_UINT, {VK_FORMAT_R16G16B16A16_UINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16B16A16_UNORM, {VK_FORMAT_R16G16B16A16_UNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16B16A16_USCALED, {VK_FORMAT_R16G16B16A16_USCALED, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16B16_FLOAT, {VK_FORMAT_R16G16B16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16B16_SINT, {VK_FORMAT_R16G16B16_SINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16B16_SNORM, {VK_FORMAT_R16G16B16_SNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16B16_SSCALED, {VK_FORMAT_R16G16B16_SSCALED, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16B16_UINT, {VK_FORMAT_R16G16B16_UINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16B16_UNORM, {VK_FORMAT_R16G16B16_UNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16B16_USCALED, {VK_FORMAT_R16G16B16_USCALED, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16_FLOAT, {VK_FORMAT_R16G16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16_SINT, {VK_FORMAT_R16G16_SINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16_SNORM, {VK_FORMAT_R16G16_SNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16_SSCALED, {VK_FORMAT_R16G16_SSCALED, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16_UINT, {VK_FORMAT_R16G16_UINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16_UNORM, {VK_FORMAT_R16G16_UNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16G16_USCALED, {VK_FORMAT_R16G16_USCALED, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16_FLOAT, {VK_FORMAT_R16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16_SINT, {VK_FORMAT_R16_SINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16_SNORM, {VK_FORMAT_R16_SNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16_SSCALED, {VK_FORMAT_R16_SSCALED, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16_UINT, {VK_FORMAT_R16_UINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16_UNORM, {VK_FORMAT_R16_UNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R16_USCALED, {VK_FORMAT_R16_USCALED, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R32G32B32A32_UNORM, {VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R32G32B32A32_FLOAT, {VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R32G32B32A32_SINT, {VK_FORMAT_R32G32B32A32_SINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R32G32B32A32_UINT, {VK_FORMAT_R32G32B32A32_UINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R32G32B32_FLOAT, {VK_FORMAT_R32G32B32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R32G32B32_SINT, {VK_FORMAT_R32G32B32_SINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R32G32B32_UINT, {VK_FORMAT_R32G32B32_UINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R32G32_FLOAT, {VK_FORMAT_R32G32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R32G32_SINT, {VK_FORMAT_R32G32_SINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R32G32_UINT, {VK_FORMAT_R32G32_UINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R32_FLOAT, {VK_FORMAT_R32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R32_SINT, {VK_FORMAT_R32_SINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R32_UINT, {VK_FORMAT_R32_UINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R4G4B4A4_UNORM, {VK_FORMAT_R4G4B4A4_UNORM_PACK16, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R5G5B5A1_UNORM, {VK_FORMAT_R5G5B5A1_UNORM_PACK16, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R5G6B5_UNORM, {VK_FORMAT_R5G6B5_UNORM_PACK16, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8B8A8_SINT, {VK_FORMAT_R8G8B8A8_SINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8B8A8_SNORM, {VK_FORMAT_R8G8B8A8_SNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8B8A8_SSCALED, {VK_FORMAT_R8G8B8A8_SSCALED, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8B8A8_UINT, {VK_FORMAT_R8G8B8A8_UINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8B8A8_UNORM, {VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8B8A8_UNORM_SRGB, {VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8B8A8_USCALED, {VK_FORMAT_R8G8B8A8_USCALED, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8B8_SINT, {VK_FORMAT_R8G8B8_SINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8B8_SNORM, {VK_FORMAT_R8G8B8_SNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8B8_SSCALED, {VK_FORMAT_R8G8B8_SSCALED, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8B8_UINT, {VK_FORMAT_R8G8B8_UINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8B8_UNORM_SRGB, {VK_FORMAT_R8G8B8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8B8_USCALED, {VK_FORMAT_R8G8B8_USCALED, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8_SINT, {VK_FORMAT_R8G8_SINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8_SNORM, {VK_FORMAT_R8G8_SNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8_SSCALED, {VK_FORMAT_R8G8_SSCALED, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8_UINT, {VK_FORMAT_R8G8_UINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8_UNORM, {VK_FORMAT_R8G8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8_UNORM_SRGB, {VK_FORMAT_R8G8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8G8_USCALED, {VK_FORMAT_R8G8_USCALED, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8_SINT, {VK_FORMAT_R8_SINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8_SNORM, {VK_FORMAT_R8_SNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8_SSCALED, {VK_FORMAT_R8_SSCALED, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8_UINT, {VK_FORMAT_R8_UINT, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8_UNORM, {VK_FORMAT_R8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8_UNORM_SRGB, {VK_FORMAT_R8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R8_USCALED, {VK_FORMAT_R8_USCALED, VK_IMAGE_ASPECT_COLOR_BIT}},
    {rhi::Format::R9G9B9E5_SHAREDEXP, {VK_FORMAT_E5B9G9R9_UFLOAT_PACK32, VK_IMAGE_ASPECT_COLOR_BIT}},
    // Depth
    {rhi::Format::D16_UNORM, {VK_FORMAT_D16_UNORM, VK_IMAGE_ASPECT_DEPTH_BIT}},
    {rhi::Format::D24_UNORM_X8_UINT, {VK_FORMAT_X8_D24_UNORM_PACK32, VK_IMAGE_ASPECT_DEPTH_BIT}},
    {rhi::Format::D32_FLOAT, {VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT}},
    // Stencil
    {rhi::Format::S8_UINT, {VK_FORMAT_S8_UINT, VK_IMAGE_ASPECT_STENCIL_BIT}},
    // DS
    {rhi::Format::D24_UNORM_S8_UINT,
     {VK_FORMAT_D24_UNORM_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT}},
    {rhi::Format::D32_FLOAT_S8X24_UINT,
     {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT}}};
} // namespace vk