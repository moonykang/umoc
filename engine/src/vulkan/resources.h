#pragma once

#include "resource/format.h"
#include "resource/image.h"
#include "resource/renderpassInfo.h"
#include "rhi/resources.h"

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
    }
}
/*
typedef enum VkLogicOp {
    VK_LOGIC_OP_CLEAR = 0,
    VK_LOGIC_OP_AND = 1,
    VK_LOGIC_OP_AND_REVERSE = 2,
    VK_LOGIC_OP_COPY = 3,
    VK_LOGIC_OP_AND_INVERTED = 4,
    VK_LOGIC_OP_NO_OP = 5,
    VK_LOGIC_OP_XOR = 6,
    VK_LOGIC_OP_OR = 7,
    VK_LOGIC_OP_NOR = 8,
    VK_LOGIC_OP_EQUIVALENT = 9,
    VK_LOGIC_OP_INVERT = 10,
    VK_LOGIC_OP_OR_REVERSE = 11,
    VK_LOGIC_OP_COPY_INVERTED = 12,
    VK_LOGIC_OP_OR_INVERTED = 13,
    VK_LOGIC_OP_NAND = 14,
    VK_LOGIC_OP_SET = 15,
*/
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
    }
}

inline VkShaderStageFlags convertToVkShaderStage(rhi::ShaderStage shaderStage)
{
    switch (shaderStage)
    {
    case rhi::ShaderStage::Vertex:
        return VK_SHADER_STAGE_VERTEX_BIT;
    case rhi::ShaderStage::Pixel:
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    }
}