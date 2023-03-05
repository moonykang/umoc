#include "screenPass.h"
#include "model/instance.h"
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
#include "scene/view.h"

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

class TriangleShaderParameters : public rhi::ShaderParameters
{
  public:
    TriangleShaderParameters() : ShaderParameters(), globalDescriptor(nullptr), localDescriptor(nullptr)
    {
    }

    std::vector<rhi::DescriptorSet*> getDescriptorSets() override
    {
        return {globalDescriptor, localDescriptor};
    }
    rhi::DescriptorSet* globalDescriptor;
    rhi::DescriptorSet* localDescriptor;
};

TriangleVertexShader triangleVertexShader;
TriangleFragmentShader trianglePixelShader;

Result ScreenPass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = platformContext->getRHI();
    scene::TestScene* testScene = reinterpret_cast<scene::TestScene*>(sceneInfo);

    static bool oneTimeRun = false; // TODO

    if (!oneTimeRun)
    {
        oneTimeRun = true;
        triangleVertexShader.init(context);
        trianglePixelShader.init(context);
    }

    rhi::RenderPassInfo renderpassInfo;
    rhi::AttachmentId attachmentId = renderpassInfo.registerColorAttachment(
        {context->getCurrentSurfaceImage(), rhi::AttachmentLoadOp::Clear, rhi::AttachmentStoreOp::Store, 1,
         rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

    auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
    subpass.colorAttachmentReference.push_back({attachmentId, rhi::ImageLayout::ColorAttachment});

    try(context->beginRenderpass(renderpassInfo));

    TriangleShaderParameters params;
    params.vertexShader = &triangleVertexShader;
    params.pixelShader = &trianglePixelShader;
    params.globalDescriptor = sceneInfo->getView()->getDescriptorSet();
    params.localDescriptor = testScene->instance->getDescriptorSet();

    rhi::GraphicsPipelineState graphicsPipelineState;
    graphicsPipelineState.shaderParameters = &params;
    graphicsPipelineState.colorBlendState.attachmentCount = 1;
    graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
    graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::NONE;
    context->createGfxPipeline(graphicsPipelineState);

    sceneInfo->getView()->getDescriptorSet()->bind(context, 0);
    testScene->instance->getDescriptorSet()->bind(context, 1);
    testScene->quad->draw(context);

    try(context->endRenderpass());

    return Result::Continue;
}
} // namespace renderer