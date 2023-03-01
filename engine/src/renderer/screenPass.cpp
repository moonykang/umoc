#include "screenPass.h"
#include "model/vertexInput.h"
#include "rhi/context.h"
#include "rhi/defines.h"
#include "rhi/rendertarget.h"
#include "rhi/resources.h"
#include "rhi/shader.h"
#include "scene/scene.h"
#include "scene/testScene.h"

namespace renderer
{
class ScreenPassVertexShader : public rhi::VertexShaderBase
{
  public:
    ScreenPassVertexShader()
        : rhi::VertexShaderBase("screen.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv)
    {
    }
};

class TriangleVertexShader : public rhi::VertexShaderBase
{
  public:
    TriangleVertexShader()
        : rhi::VertexShaderBase("triangle.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Color)
    {
    }
};

class TriangleFragmentShader : public rhi::PixelShaderBase
{
  public:
    TriangleFragmentShader() : rhi::PixelShaderBase("triangle.frag.spv")
    {
    }
};

class TriangleShaderContainer : public rhi::ShaderContainer
{
  public:
    TriangleShaderContainer(rhi::VertexShaderBase* vertexShader, rhi::PixelShaderBase* pixelShader)
        : rhi::ShaderContainer()
    {
        this->vertexShader = vertexShader;
        this->pixelShader = pixelShader;
    }

    std::vector<rhi::DescriptorInfoList> getDescriptorListSet()
    {
        uint32_t numSets = 1;
        std::vector<rhi::DescriptorInfoList> descriptorInfoLists;
        descriptorInfoLists.reserve(numSets);

        rhi::DescriptorInfoList& descriptorInfoList = descriptorInfoLists[0];
        descriptorInfoList.push_back({0, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler});

        return descriptorInfoLists;
    }
};

TriangleVertexShader triangleVertexShader;
TriangleFragmentShader trianglePixelShader;
TriangleShaderContainer triangleShaderContainer(&triangleVertexShader, &trianglePixelShader);

ScreenPassVertexShader screenPassVertexShader;

Result ScreenPass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();

    // screenPassVertexShader.loadShader(context->getRHI());

    triangleShaderContainer.init(context->getRHI());

    rhi::RenderPassInfo renderpassInfo;
    rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
        {context->getCurrentSurfaceImage(), rhi::AttachmentLoadOp::Clear, rhi::AttachmentStoreOp::Store, 1,
         rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

    auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
    subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});

    try(context->beginRenderpass(renderpassInfo));

    rhi::GraphicsPipelineState graphicsPipelineState;
    graphicsPipelineState.shaderContainer = &triangleShaderContainer;
    graphicsPipelineState.colorBlendState.attachmentCount = 1;
    graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
    graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::NONE;
    context->createGfxPipeline(graphicsPipelineState);

    scene::TestScene* testScene = reinterpret_cast<scene::TestScene*>(sceneInfo);
    testScene->quad->draw(context);

    try(context->endRenderpass());

    return Result::Continue;
}
} // namespace renderer