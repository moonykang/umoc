#include "screenPass.h"
#include "model/vertexInput.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/defines.h"
#include "rhi/descriptor.h"
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

    rhi::DescriptorInfoListSet getDescriptorListSet()
    {
        return TriangleShaderParameters::getDescriptorInfos();
    }

    class TriangleShaderParameters : public ShaderParameters
    {
      public:
        TriangleShaderParameters() : globalDescriptor(nullptr)
        {
        }

        static rhi::DescriptorInfoListSet getDescriptorInfos()
        {
            rhi::DescriptorInfoListSet descriptorInfos(numSets);

            fillDescriptorInfo(descriptorInfos, 0, 0, rhi::ShaderStage::Vertex,
                               rhi::DescriptorType::Uniform_Buffer_Dynamic);

            return descriptorInfos;
        };

        rhi::DescriptorListSet getDescriptors()
        {
            rhi::DescriptorListSet descriptors(numSets);

            fillDescriptor(descriptors, 0, 0, rhi::ShaderStage::Vertex, rhi::DescriptorType::Uniform_Buffer_Dynamic,
                           globalDescriptor);
            return descriptors;
        }

      public:
        static const uint32_t numSets = 1;
        rhi::Descriptor* globalDescriptor;
    };

    // set: # binding # type # stage # descriptor*
};

TriangleVertexShader triangleVertexShader;
TriangleFragmentShader trianglePixelShader;
TriangleShaderContainer triangleShaderContainer(&triangleVertexShader, &trianglePixelShader);

ScreenPassVertexShader screenPassVertexShader;

Result ScreenPass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();
    scene::TestScene* testScene = reinterpret_cast<scene::TestScene*>(sceneInfo);

    static bool oneTimeRun = false; // TODO

    if (!oneTimeRun)
    {
        oneTimeRun = true;
        triangleShaderContainer.init(context->getRHI());
        testScene->descriptorSet->init(context, triangleShaderContainer.getDescriptorLayout(0));
    }

    rhi::RenderPassInfo renderpassInfo;
    rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
        {context->getCurrentSurfaceImage(), rhi::AttachmentLoadOp::Clear, rhi::AttachmentStoreOp::Store, 1,
         rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

    auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
    subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});

    try(context->beginRenderpass(renderpassInfo));

    TriangleShaderContainer::TriangleShaderParameters params;
    params.globalDescriptor = testScene->uniformBuffer->getDescriptor();
    testScene->descriptorSet->update(context, params.getDescriptors()[0]);

    rhi::GraphicsPipelineState graphicsPipelineState;
    graphicsPipelineState.shaderContainer = &triangleShaderContainer;
    graphicsPipelineState.colorBlendState.attachmentCount = 1;
    graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
    graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::NONE;
    context->createGfxPipeline(graphicsPipelineState);

    testScene->descriptorSet->bind(context, 0);
    testScene->quad->draw(context);

    try(context->endRenderpass());

    return Result::Continue;
}
} // namespace renderer