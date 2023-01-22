#include "common/hash.h"
#include "common/util.h"
#include "rhi/resources.h"
#include <gtest/gtest.h>

using namespace rhi;

TEST(PipelineStateFixture, RasterizationStateTest)
{
    RasterizationState rasterizationState;

    size_t totalSize =
        (4 * sizeof(float)) + sizeof(PolygonMode) + sizeof(CullMode) + (3 * sizeof(bool)) + sizeof(FrontFace);

    size_t offset = 0;
    EXPECT_EQ(offsetof(RasterizationState, depthBiasConstantFactor), offset);
    offset += sizeof(float);
    EXPECT_EQ(offsetof(RasterizationState, depthBiasClamp), offset);
    offset += sizeof(float);
    EXPECT_EQ(offsetof(RasterizationState, depthBiasSlopeFactor), offset);
    offset += sizeof(float);
    EXPECT_EQ(offsetof(RasterizationState, lineWidth), offset);
    offset += sizeof(float);
    EXPECT_EQ(offsetof(RasterizationState, polygonMode), offset);
    offset += sizeof(PolygonMode);
    EXPECT_EQ(offsetof(RasterizationState, cullMode), offset);
    offset += sizeof(CullMode);
    EXPECT_EQ(offsetof(RasterizationState, depthClampEnable), offset);
    offset += sizeof(bool);
    EXPECT_EQ(offsetof(RasterizationState, rasterizerDiscardEnable), offset);
    offset += sizeof(bool);
    EXPECT_EQ(offsetof(RasterizationState, depthBiasEnable), offset);
    offset += sizeof(bool);
    EXPECT_EQ(offsetof(RasterizationState, frontFace), offset);
    offset += sizeof(FrontFace);

    EXPECT_EQ(sizeof(PolygonMode), sizeof(uint8_t));
    EXPECT_EQ(sizeof(CullMode), sizeof(uint8_t));
    EXPECT_EQ(sizeof(FrontFace), sizeof(bool));

    RasterizationState rasterizationState2;
    rasterizationState2.cullMode = CullMode::CULL_MODE_FRONT_BIT;

    size_t hash1 = util::computeGenericHash(&rasterizationState, sizeof(RasterizationState));
    size_t hash2 = util::computeGenericHash(&rasterizationState2, sizeof(RasterizationState));

    EXPECT_NE(hash1, hash2);
}

TEST(PipelineStateFixture, MultisampleStateTest)
{
    MultisampleState multisampleState;

    size_t offset = 0;
    EXPECT_EQ(offsetof(MultisampleState, minSampleShading), offset);
    offset += sizeof(float);
    EXPECT_EQ(offsetof(MultisampleState, sampleCount), offset);
    offset += sizeof(SampleCount);
    EXPECT_EQ(offsetof(MultisampleState, sampleShadingEnable), offset);
    offset += sizeof(bool);
    EXPECT_EQ(offsetof(MultisampleState, alphaToCoverageEnable), offset);
    offset += sizeof(bool);
    EXPECT_EQ(offsetof(MultisampleState, alphaToOneEnable), offset);
    offset += sizeof(bool);

    MultisampleState multisampleState2;
    multisampleState2.minSampleShading = 1.f;

    size_t hash1 = util::computeGenericHash(&multisampleState, sizeof(MultisampleState));
    size_t hash2 = util::computeGenericHash(&multisampleState2, sizeof(MultisampleState));

    EXPECT_NE(hash1, hash2);
}

TEST(PipelineStateFixture, StencilOpStateTest)
{
    StencilOpState stencilOpState;

    size_t offset = 0;
    EXPECT_EQ(offsetof(StencilOpState, failOp), offset);
    offset += sizeof(StencilOp);
    EXPECT_EQ(offsetof(StencilOpState, passOp), offset);
    offset += sizeof(StencilOp);
    EXPECT_EQ(offsetof(StencilOpState, depthFailOp), offset);
    offset += sizeof(StencilOp);
    EXPECT_EQ(offsetof(StencilOpState, compareOp), offset);
    offset += sizeof(CompareOp);
    EXPECT_EQ(offsetof(StencilOpState, compareMask), offset);
    offset += sizeof(uint32_t);
    EXPECT_EQ(offsetof(StencilOpState, writeMask), offset);
    offset += sizeof(uint32_t);
    EXPECT_EQ(offsetof(StencilOpState, reference), offset);
    offset += sizeof(uint32_t);

    StencilOpState stencilOpState2;
    stencilOpState2.failOp = StencilOp::INVERT;

    size_t hash1 = util::computeGenericHash(&stencilOpState, sizeof(StencilOpState));
    size_t hash2 = util::computeGenericHash(&stencilOpState2, sizeof(StencilOpState));

    EXPECT_NE(hash1, hash2);
}

TEST(PipelineStateFixture, DepthStencilStateTest)
{
    DepthStencilState depthStencilState;

    size_t offset = 0;
    EXPECT_EQ(offsetof(DepthStencilState, front), offset);
    offset += sizeof(StencilOpState);
    EXPECT_EQ(offsetof(DepthStencilState, back), offset);
    offset += sizeof(StencilOpState);
    EXPECT_EQ(offsetof(DepthStencilState, minDepthBounds), offset);
    offset += sizeof(float);
    EXPECT_EQ(offsetof(DepthStencilState, maxDepthBounds), offset);
    offset += sizeof(float);
    EXPECT_EQ(offsetof(DepthStencilState, depthCompareOp), offset);
    offset += sizeof(CompareOp);
    EXPECT_EQ(offsetof(DepthStencilState, depthTestEnable), offset);
    offset += sizeof(bool);
    EXPECT_EQ(offsetof(DepthStencilState, depthWriteEnable), offset);
    offset += sizeof(bool);
    EXPECT_EQ(offsetof(DepthStencilState, depthBoundsTestEnable), offset);
    offset += sizeof(bool);
    EXPECT_EQ(offsetof(DepthStencilState, stencilTestEnable), offset);
    offset += sizeof(bool);

    DepthStencilState depthStencilState2;
    depthStencilState2.minDepthBounds = 1.f;

    size_t hash1 = util::computeGenericHash(&depthStencilState, sizeof(DepthStencilState));
    size_t hash2 = util::computeGenericHash(&depthStencilState2, sizeof(DepthStencilState));

    EXPECT_NE(hash1, hash2);
}

TEST(PipelineStateFixture, ColorBlendStateTest)
{

    size_t offset = 0;
    EXPECT_EQ(offsetof(ColorBlendState, srcColorBlendFactor), offset);
    offset += sizeof(BlendFactor);
    EXPECT_EQ(offsetof(ColorBlendState, dstColorBlendFactor), offset);
    offset += sizeof(BlendFactor);
    EXPECT_EQ(offsetof(ColorBlendState, srcAlphaBlendFactor), offset);
    offset += sizeof(BlendFactor);
    EXPECT_EQ(offsetof(ColorBlendState, dstAlphaBlendFactor), offset);
    offset += sizeof(BlendFactor);
    EXPECT_EQ(offsetof(ColorBlendState, colorBlendOp), offset);
    offset += sizeof(BlendOp);
    EXPECT_EQ(offsetof(ColorBlendState, alphaBlendOp), offset);
    offset += sizeof(BlendOp);
    EXPECT_EQ(offsetof(ColorBlendState, colorWriteMask), offset);
    offset += sizeof(ColorComponentFlags);
    EXPECT_EQ(offsetof(ColorBlendState, blendEnable), offset);
    offset += sizeof(bool);

    ColorBlendState colorBlendState;
    ColorBlendState colorBlendState2;
    colorBlendState2.blendEnable = true;

    size_t hash1 = util::computeGenericHash(&colorBlendState, sizeof(ColorBlendState));
    size_t hash2 = util::computeGenericHash(&colorBlendState2, sizeof(ColorBlendState));

    EXPECT_NE(hash1, hash2);
}