#include "common/util.h"
#include "rhi/resources.h"
#include "rhi/shader.h"
#include <gtest/gtest.h>

using namespace rhi;

class TestShaderContainer : public rhi::ShaderContainer
{
  public:
    std::vector<DescriptorInfoList> getDescriptorListSet() override
    {
        std::vector<DescriptorInfoList> descriptorInfoLists;

        rhi::DescriptorInfoList descriptorInfoList;
        descriptorInfoList.push_back({0, ShaderStage::Vertex, DescriptorType::Sampler});
        descriptorInfoList.push_back(
            {3, ShaderStage::Vertex | ShaderStage::Pixel, DescriptorType::Combined_Image_Sampler});

        descriptorInfoLists.push_back(std::move(descriptorInfoList));
        return descriptorInfoLists;
    }
};

TEST(DescriptorFixture, DescriptorInfoTest)
{
    TestShaderContainer testShaderContainer;
    auto descriptorInfoSet = testShaderContainer.getDescriptorListSet();
    DescriptorInfoList& descriptorInfos = descriptorInfoSet.at(0);
    EXPECT_EQ(2, descriptorInfos.size());

    {
        auto& descriptorInfo = descriptorInfos.at(0);
        EXPECT_EQ(0, descriptorInfo.getBinding());
        EXPECT_EQ(ShaderStage::Vertex, descriptorInfo.getShaderStage());
        EXPECT_EQ(DescriptorType::Sampler, descriptorInfo.getType());
    }

    {
        auto& descriptorInfo = descriptorInfos.at(1);
        EXPECT_EQ(3, descriptorInfo.getBinding());
        EXPECT_EQ(ShaderStage::Vertex | ShaderStage::Pixel, descriptorInfo.getShaderStage());
        EXPECT_EQ(DescriptorType::Combined_Image_Sampler, descriptorInfo.getType());
    }
}