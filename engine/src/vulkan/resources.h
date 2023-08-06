#pragma once

#include "resource/format.h"
#include "resource/image.h"
#include "resource/renderpassInfo.h"
#include "rhi/resources.h"

inline VkIndexType convertToVkIndexType(rhi::IndexType indexType)
{
    switch (indexType)
    {
    case rhi::IndexType::UINT8:
        return VkIndexType::VK_INDEX_TYPE_UINT8_EXT;
    case rhi::IndexType::UINT16:
        return VkIndexType::VK_INDEX_TYPE_UINT16;
    case rhi::IndexType::UINT32:
        return VkIndexType::VK_INDEX_TYPE_UINT32;
    default:
        UNREACHABLE();
        return VkIndexType::VK_INDEX_TYPE_NONE_KHR;
    }
}

inline VkPrimitiveTopology convertToVkPrimitiveTopology(rhi::PrimitiveTopology topology)
{
    switch (topology)
    {
    case rhi::PrimitiveTopology::LINE_LIST:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case rhi::PrimitiveTopology::LINE_LIST_WITH_ADJACENCY:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
    case rhi::PrimitiveTopology::LINE_STRIP:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    case rhi::PrimitiveTopology::LINE_STRIP_WITH_ADJACENCY:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
    case rhi::PrimitiveTopology::PATCH_LIST:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
    case rhi::PrimitiveTopology::POINT_LIST:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    case rhi::PrimitiveTopology::TRIANGLE_FAN:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
    case rhi::PrimitiveTopology::TRIANGLE_LIST:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case rhi::PrimitiveTopology::TRIANGLE_LIST_WITH_ADJACENCY:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
    case rhi::PrimitiveTopology::TRIANGLE_STRIP:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    case rhi::PrimitiveTopology::TRIANGLE_STRIP_WITH_ADJACENCY:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
    default:
        UNREACHABLE();
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
}

inline VkCullModeFlags convertToVkCullMode(rhi::CullMode cullMode)
{
    switch (cullMode)
    {
    case rhi::CullMode::NONE:
        return VK_CULL_MODE_NONE;
    case rhi::CullMode::FRONT_BIT:
        return VK_CULL_MODE_FRONT_BIT;
    case rhi::CullMode::BACK_BIT:
        return VK_CULL_MODE_BACK_BIT;
    case rhi::CullMode::FRONT_AND_BACK:
        return VK_CULL_MODE_FRONT_AND_BACK;
    default:
        UNREACHABLE();
        return VK_CULL_MODE_NONE;
    }
}

inline VkPolygonMode convertToVkPolygonMode(rhi::PolygonMode polygonMode)
{
    switch (polygonMode)
    {
    case rhi::PolygonMode::FILL:
        return VK_POLYGON_MODE_FILL;
    case rhi::PolygonMode::LINE:
        return VK_POLYGON_MODE_LINE;
    case rhi::PolygonMode::POINT:
        return VK_POLYGON_MODE_POINT;
    default:
        UNREACHABLE();
        return VK_POLYGON_MODE_FILL;
    }
}

inline VkFrontFace convertToVkFrontFace(rhi::FrontFace frontFace)
{
    switch (frontFace)
    {
    case rhi::FrontFace::COUNTER_CLOCKWISE:
        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    case rhi::FrontFace::CLOCKWISE:
        return VK_FRONT_FACE_CLOCKWISE;
    default:
        UNREACHABLE();
        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }
}

inline VkSampleCountFlagBits convertToVkSampleCountBits(rhi::SampleCount sampleCount)
{
    switch (sampleCount)
    {
    case rhi::SampleCount::Sample_1:
        return VK_SAMPLE_COUNT_1_BIT;
    case rhi::SampleCount::Sample_2:
        return VK_SAMPLE_COUNT_2_BIT;
    case rhi::SampleCount::Sample_4:
        return VK_SAMPLE_COUNT_4_BIT;
    case rhi::SampleCount::Sample_8:
        return VK_SAMPLE_COUNT_8_BIT;
    case rhi::SampleCount::Sample_16:
        return VK_SAMPLE_COUNT_16_BIT;
    case rhi::SampleCount::Sample_32:
        return VK_SAMPLE_COUNT_32_BIT;
    case rhi::SampleCount::Sample_64:
        return VK_SAMPLE_COUNT_64_BIT;
    default:
        UNREACHABLE();
        return VK_SAMPLE_COUNT_1_BIT;
    }
}

inline VkCompareOp convertToVkCompareOp(rhi::CompareOp sompareOp)
{
    switch (sompareOp)
    {
    case rhi::CompareOp::NEVER:
        return VK_COMPARE_OP_NEVER;
    case rhi::CompareOp::LESS:
        return VK_COMPARE_OP_LESS;
    case rhi::CompareOp::EQUAL:
        return VK_COMPARE_OP_EQUAL;
    case rhi::CompareOp::LESS_OR_EQUAL:
        return VK_COMPARE_OP_LESS_OR_EQUAL;
    case rhi::CompareOp::GREATER:
        return VK_COMPARE_OP_GREATER;
    case rhi::CompareOp::NOT_EQUAL:
        return VK_COMPARE_OP_NOT_EQUAL;
    case rhi::CompareOp::GREATER_OR_EQUAL:
        return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case rhi::CompareOp::ALWAYS:
        return VK_COMPARE_OP_ALWAYS;
    default:
        UNREACHABLE();
        return VK_COMPARE_OP_NEVER;
    }
}

inline VkStencilOp convertToVkStencilOp(rhi::StencilOp stencilOp)
{
    switch (stencilOp)
    {
    case rhi::StencilOp::KEEP:
        return VK_STENCIL_OP_KEEP;
    case rhi::StencilOp::ZERO:
        return VK_STENCIL_OP_ZERO;
    case rhi::StencilOp::REPLACE:
        return VK_STENCIL_OP_REPLACE;
    case rhi::StencilOp::INCREMENT_AND_CLAMP:
        return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
    case rhi::StencilOp::DECREMENT_AND_CLAMP:
        return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
    case rhi::StencilOp::INVERT:
        return VK_STENCIL_OP_INVERT;
    case rhi::StencilOp::INCREMENT_AND_WRAP:
        return VK_STENCIL_OP_INCREMENT_AND_WRAP;
    case rhi::StencilOp::DECREMENT_AND_WRAP:
        return VK_STENCIL_OP_DECREMENT_AND_WRAP;
    default:
        UNREACHABLE();
        return VK_STENCIL_OP_KEEP;
    }
}

inline VkBlendFactor convertToVkBlendFactor(rhi::BlendFactor blendFactor)
{
    switch (blendFactor)
    {
    case rhi::BlendFactor::ZERO:
        return VK_BLEND_FACTOR_ZERO;
    case rhi::BlendFactor::ONE:
        return VK_BLEND_FACTOR_ONE;
    case rhi::BlendFactor::SRC_COLOR:
        return VK_BLEND_FACTOR_SRC_COLOR;
    case rhi::BlendFactor::ONE_MINUS_SRC_COLOR:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    case rhi::BlendFactor::DST_COLOR:
        return VK_BLEND_FACTOR_DST_COLOR;
    case rhi::BlendFactor::ONE_MINUS_DST_COLOR:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case rhi::BlendFactor::SRC_ALPHA:
        return VK_BLEND_FACTOR_SRC_ALPHA;
    case rhi::BlendFactor::ONE_MINUS_SRC_ALPHA:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case rhi::BlendFactor::DST_ALPHA:
        return VK_BLEND_FACTOR_DST_ALPHA;
    case rhi::BlendFactor::ONE_MINUS_DST_ALPHA:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    case rhi::BlendFactor::CONSTANT_COLOR:
        return VK_BLEND_FACTOR_CONSTANT_COLOR;
    case rhi::BlendFactor::ONE_MINUS_CONSTANT_COLOR:
        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
    case rhi::BlendFactor::CONSTANT_ALPHA:
        return VK_BLEND_FACTOR_CONSTANT_ALPHA;
    case rhi::BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
    case rhi::BlendFactor::SRC_ALPHA_SATURATE:
        return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
    case rhi::BlendFactor::SRC1_COLOR:
        return VK_BLEND_FACTOR_SRC1_COLOR;
    case rhi::BlendFactor::ONE_MINUS_SRC1_COLOR:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
    case rhi::BlendFactor::SRC1_ALPHA:
        return VK_BLEND_FACTOR_SRC1_ALPHA;
    case rhi::BlendFactor::ONE_MINUS_SRC1_ALPHA:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
    default:
        UNREACHABLE();
        return VK_BLEND_FACTOR_ZERO;
    }
}

inline VkBlendOp convertToVkBlendFOp(rhi::BlendOp blendOp)
{
    switch (blendOp)
    {
    case rhi::BlendOp::ADD:
        return VK_BLEND_OP_ADD;
    case rhi::BlendOp::SUBTRACT:
        return VK_BLEND_OP_SUBTRACT;
    case rhi::BlendOp::REVERSE_SUBTRACT:
        return VK_BLEND_OP_REVERSE_SUBTRACT;
    case rhi::BlendOp::MIN:
        return VK_BLEND_OP_MIN;
    case rhi::BlendOp::MAX:
        return VK_BLEND_OP_MAX;
    default:
        UNREACHABLE();
        return VK_BLEND_OP_ADD;
    }
}

inline VkLogicOp convertToVkLogicOp(rhi::LogicOp logicOp)
{
    switch (logicOp)
    {
    case rhi::LogicOp::CLEAR:
        return VK_LOGIC_OP_CLEAR;
    case rhi::LogicOp::AND:
        return VK_LOGIC_OP_AND;
    case rhi::LogicOp::REVERSE:
        return VK_LOGIC_OP_AND_REVERSE;
    case rhi::LogicOp::COPY:
        return VK_LOGIC_OP_COPY;
    case rhi::LogicOp::AND_INVERTED:
        return VK_LOGIC_OP_AND_INVERTED;
    case rhi::LogicOp::NO_OP:
        return VK_LOGIC_OP_NO_OP;
    case rhi::LogicOp::XOR:
        return VK_LOGIC_OP_XOR;
    case rhi::LogicOp::OR:
        return VK_LOGIC_OP_OR;
    case rhi::LogicOp::NOR:
        return VK_LOGIC_OP_NOR;
    case rhi::LogicOp::EQUIVALENT:
        return VK_LOGIC_OP_EQUIVALENT;
    case rhi::LogicOp::INVERT:
        return VK_LOGIC_OP_INVERT;
    case rhi::LogicOp::OR_REVERSE:
        return VK_LOGIC_OP_OR_REVERSE;
    case rhi::LogicOp::COPY_INVERTED:
        return VK_LOGIC_OP_COPY_INVERTED;
    case rhi::LogicOp::OR_INVERTED:
        return VK_LOGIC_OP_OR_INVERTED;
    case rhi::LogicOp::NAND:
        return VK_LOGIC_OP_NAND;
    case rhi::LogicOp::SET:
        return VK_LOGIC_OP_SET;
    default:
        UNREACHABLE();
        return VK_LOGIC_OP_CLEAR;
    }
}

inline VkShaderStageFlags convertToVkShaderStage(rhi::ShaderStageFlags shaderStage)
{
    VkShaderStageFlags shaderStageFlags = 0;
    if ((shaderStage & rhi::ShaderStage::Vertex) != 0)
    {
        shaderStageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
    }
    if ((shaderStage & rhi::ShaderStage::Geometry) != 0)
    {
        shaderStageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
    }
    if ((shaderStage & rhi::ShaderStage::TessellationControl) != 0)
    {
        shaderStageFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    }
    if ((shaderStage & rhi::ShaderStage::TessellationEvaluation) != 0)
    {
        shaderStageFlags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    }
    if ((shaderStage & rhi::ShaderStage::Pixel) != 0)
    {
        shaderStageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
    }
    if ((shaderStage & rhi::ShaderStage::Compute) != 0)
    {
        shaderStageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
    }
    if ((shaderStage & rhi::ShaderStage::RayGen) != 0)
    {
        shaderStageFlags |= VK_SHADER_STAGE_RAYGEN_BIT_KHR;
    }
    if ((shaderStage & rhi::ShaderStage::RayMiss) != 0)
    {
        shaderStageFlags |= VK_SHADER_STAGE_MISS_BIT_KHR;
    }
    if ((shaderStage & rhi::ShaderStage::ClosestHit) != 0)
    {
        shaderStageFlags |= VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    }
    return shaderStageFlags;
}

inline VkDescriptorType convertToVkDescriptorType(rhi::DescriptorType descriptorType)
{
    switch (descriptorType)
    {
    case rhi::DescriptorType::Sampler:
        return VK_DESCRIPTOR_TYPE_SAMPLER;
    case rhi::DescriptorType::Combined_Image_Sampler:
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    case rhi::DescriptorType::Sampled_Image:
        return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    case rhi::DescriptorType::Storage_Image:
        return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    case rhi::DescriptorType::Uniform_Texel_Buffer:
        return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    case rhi::DescriptorType::Storage_Texel_Buffer:
        return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    case rhi::DescriptorType::Uniform_Buffer:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case rhi::DescriptorType::Storage_Buffer:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case rhi::DescriptorType::Uniform_Buffer_Dynamic:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    case rhi::DescriptorType::Storage_Buffer_Dynamic:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    case rhi::DescriptorType::Input_Attachment:
        return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    case rhi::DescriptorType::Acceleration_structure:
        return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    }
}

inline VkImageType convertToVkImageType(rhi::ImageType imageType)
{
    switch (imageType)
    {
    case rhi::ImageType::IMAGE_1D:
        return VK_IMAGE_TYPE_1D;
    case rhi::ImageType::IMAGE_2D:
        return VK_IMAGE_TYPE_2D;
    case rhi::ImageType::IMAGE_3D:
        return VK_IMAGE_TYPE_3D;
    case rhi::ImageType::IMAGE_CUBE:
        return VK_IMAGE_TYPE_2D;
    default:
        UNREACHABLE();
        return VK_IMAGE_TYPE_1D;
    }
}