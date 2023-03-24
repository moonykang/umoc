#include "screenPass.h"
#include "model/instance.h"
#include "model/material.h"
#include "model/object.h"
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
        : rhi::VertexShaderBase("triangle.vert.spv",
                                rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal)
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
    TriangleShaderParameters()
        : ShaderParameters(), globalDescriptor(nullptr), localDescriptor(nullptr), materialDescriptor(nullptr)
    {
    }

    std::vector<rhi::DescriptorSet*> getDescriptorSets() override
    {
        return {globalDescriptor, localDescriptor, materialDescriptor};
    }
    rhi::DescriptorSet* globalDescriptor;
    rhi::DescriptorSet* localDescriptor;
    rhi::DescriptorSet* materialDescriptor;
};

TriangleVertexShader triangleVertexShader;
TriangleFragmentShader trianglePixelShader;

Result ScreenPass::init(platform::Context* context, scene::SceneInfo* sceneInfo)
{
    return Result::Continue;
}

void ScreenPass::terminate(platform::Context* context)
{
}

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
    params.globalDescriptor = sceneInfo->getDescriptorSet();

    rhi::GraphicsPipelineState graphicsPipelineState;
    graphicsPipelineState.shaderParameters = &params;
    graphicsPipelineState.colorBlendState.attachmentCount = 1;
    graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
    graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::NONE;

    for (auto& model : testScene->getModels())
    {
        for (auto& instance : model->getInstances())
        {
            auto materialDescriptor = instance->getMaterial()->getDescriptorSet();

            params.materialDescriptor = materialDescriptor;
            params.localDescriptor = instance->getDescriptorSet();
            context->createGfxPipeline(graphicsPipelineState);

            sceneInfo->getDescriptorSet()->bind(context, 0);
            instance->getDescriptorSet()->bind(context, 1);
            materialDescriptor->bind(context, 2);

            model->draw(context);
            instance->draw(context);
        }
    }

    try(context->endRenderpass());

    return Result::Continue;
}
} // namespace renderer