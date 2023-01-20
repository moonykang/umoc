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