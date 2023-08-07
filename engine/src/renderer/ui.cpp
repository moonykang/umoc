#include "ui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "model/material.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/defines.h"
#include "rhi/descriptor.h"
#include "rhi/image.h"
#include "rhi/shader.h"
#include "scene/rendertargets.h"
#include "scene/scene.h"
#include "scene/view.h"

struct UIPushBlock
{
    glm::vec2 scale;
    glm::vec2 translate;
} uiPushBlock;

namespace renderer
{
namespace ui
{

const size_t VERTEX_SCRATCH_BUFFER_SIZE = 16 * 1024 * 1024;
const size_t INDEX_SCRATCH_BUFFER_SIZE = 16 * 1024 * 1024;
} // namespace ui
UIPass::UIPass() : vertexScratchBuffer(nullptr), indexScratchBuffer(nullptr)
{
}

Result UIPass::init(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    {
        vertexScratchBuffer = new rhi::ScratchBuffer();
        vertexScratchBuffer->init(context, rhi::BufferUsage::VERTEX_BUFFER | rhi::BufferUsage::TRANSFER_DST,
                                  rhi::MemoryProperty::HOST_VISIBLE | rhi::MemoryProperty::HOST_COHERENT,
                                  ui::VERTEX_SCRATCH_BUFFER_SIZE);
        indexScratchBuffer = new rhi::ScratchBuffer();
        indexScratchBuffer->init(context, rhi::BufferUsage::INDEX_BUFFER | rhi::BufferUsage::TRANSFER_DST,
                                 rhi::MemoryProperty::HOST_VISIBLE | rhi::MemoryProperty::HOST_COHERENT,
                                 ui::INDEX_SCRATCH_BUFFER_SIZE);
    }

    shaderParameters = std::make_shared<rhi::ShaderParameters>();

    std::vector<rhi::VertexAttribute> vertexAttributes = {
        rhi::VertexAttribute(rhi::Format::R32G32_FLOAT, offsetof(ImDrawVert, pos)),
        rhi::VertexAttribute(rhi::Format::R32G32_FLOAT, offsetof(ImDrawVert, uv)),
        rhi::VertexAttribute(rhi::Format::R8G8B8A8_UNORM, offsetof(ImDrawVert, col))};

    shaderParameters->vertexShader =
        context->allocateVertexShader("ui/ui.vert.spv", vertexAttributes, sizeof(ImDrawVert));
    shaderParameters->pixelShader = context->allocatePixelShader("ui/ui.frag.spv");

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(1024, 1024);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    io.FontGlobalScale = 1.f;
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    style.ScaleAllSizes(1.f);

    // Create font texture
    unsigned char* fontData;
    int width, height;

    io.Fonts->GetTexDataAsRGBA32(&fontData, &width, &height);
    size_t fontSize = width * height * 4 * sizeof(char);

    fontTexture = new rhi::Texture("uiFont");
    try(fontTexture->init(context, rhi::Format::R8G8B8A8_UNORM,
                          {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1}, rhi::ImageUsage::SAMPLED,
                          fontSize, fontData));

    uiMaterial = new model::Material();
    try(uiMaterial->init(platformContext));
    uiMaterial->updateTexture(model::MaterialFlag::BaseColorTexture, fontTexture, rhi::ShaderStage::Pixel);
    try(uiMaterial->update(platformContext));

    return Result::Continue;
}

void UIPass::terminate(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    TERMINATE(vertexScratchBuffer, context);
    TERMINATE(indexScratchBuffer, context);
    TERMINATE(fontTexture, context);
    TERMINATE(uiMaterial, platformContext);
}

Result UIPass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    try(updateBuffers(platformContext));

    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    rhi::RenderPassInfo renderpassInfo;
    renderpassInfo.name = "UI pass";

    rhi::AttachmentId gBufferAattachmentId = renderpassInfo.registerColorAttachment(
        {context->getCurrentSurfaceImage(), rhi::AttachmentLoadOp::Load, rhi::AttachmentStoreOp::Store, 1,
         rhi::ImageLayout::ColorAttachment, rhi::ImageLayout::ColorAttachment});

    auto& subpass = renderpassInfo.subpassDescriptions.emplace_back();
    subpass.colorAttachmentReference.push_back({gBufferAattachmentId, rhi::ImageLayout::ColorAttachment});

    try(context->beginRenderpass(renderpassInfo));

    shaderParameters->materialDescriptor = uiMaterial->getDescriptorSet();
    uiMaterial->getDescriptorSet()->bind(context, 0);

    rhi::GraphicsPipelineState graphicsPipelineState;
    graphicsPipelineState.shaderParameters = shaderParameters.get();
    graphicsPipelineState.assemblyState.primitiveTopology = rhi::PrimitiveTopology::TRIANGLE_LIST;
    graphicsPipelineState.rasterizationState.polygonMode = rhi::PolygonMode::FILL;
    graphicsPipelineState.rasterizationState.frontFace = rhi::FrontFace::COUNTER_CLOCKWISE;
    graphicsPipelineState.rasterizationState.cullMode = rhi::CullMode::NONE;
    graphicsPipelineState.colorBlendState.attachmentCount = 1;

    graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].srcColorBlendFactor =
        rhi::BlendFactor::SRC_ALPHA;
    graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].dstColorBlendFactor =
        rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
    graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].srcAlphaBlendFactor =
        rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
    graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].dstAlphaBlendFactor = rhi::BlendFactor::ZERO;
    graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].colorBlendOp = rhi::BlendOp::ADD;
    graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].alphaBlendOp = rhi::BlendOp::ADD;
    graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].colorWriteMask =
        rhi::ColorComponent::R | rhi::ColorComponent::G | rhi::ColorComponent::B | rhi::ColorComponent::A;
    graphicsPipelineState.colorBlendState.colorBlendAttachmentStates[0].blendEnable = true;
    graphicsPipelineState.depthStencilState.depthTestEnable = false;
    graphicsPipelineState.depthStencilState.depthCompareOp = rhi::CompareOp::LESS_OR_EQUAL;
    graphicsPipelineState.depthStencilState.depthWriteEnable = false;

    graphicsPipelineState.pushConstants.push_back(rhi::PushConstant(rhi::ShaderStage::Vertex, 0, sizeof(UIPushBlock)));

    try(context->createGfxPipeline(graphicsPipelineState));

    ImGuiIO& io = ImGui::GetIO();
    UIPushBlock uiPushBlock;
    uiPushBlock.scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
    uiPushBlock.translate = glm::vec2(-1.0f);

    // Render commands
    ImDrawData* imDrawData = ImGui::GetDrawData();
    int32_t vertexOffset = 0;
    int32_t indexOffset = 0;

    if (imDrawData->CmdListsCount > 0)
    {
        context->bindVertexBuffer(vertexScratchBuffer->getBuffer(), 0);
        context->bindindexBuffer(indexScratchBuffer->getBuffer(), 0, rhi::IndexType::UINT16);

        for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
        {
            const ImDrawList* cmd_list = imDrawData->CmdLists[i];
            for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
            {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];

                int x = std::max((int32_t)(pcmd->ClipRect.x), 0);
                int y = std::max((int32_t)(pcmd->ClipRect.y), 0);
                uint32_t width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
                uint32_t height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);

                context->setScissor(x, y, width, height);

                context->pushConstant(rhi::ShaderStage::Vertex, sizeof(UIPushBlock), &uiPushBlock);
                context->drawIndexed(pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);

                indexOffset += pcmd->ElemCount;
            }
            vertexOffset += cmd_list->VtxBuffer.Size;
        }
    }

    try(context->endRenderpass());

    return Result::Continue;
}

Result UIPass::updateBuffers(platform::Context* platformContext)
{
    rhi::Context* context = platformContext->getRHI();

    ImDrawData* imDrawData = ImGui::GetDrawData();

    VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
    VkDeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

    if ((vertexBufferSize == 0) || (indexBufferSize == 0))
    {
        return Result::Continue;
    }

    if (vertexCount != imDrawData->TotalVtxCount)
    {
        vertexCount = imDrawData->TotalVtxCount;
    }

    if (indexCount != imDrawData->TotalIdxCount)
    {
        indexCount = imDrawData->TotalIdxCount;
    }

    size_t vertexOffset = 0;
    size_t indexOffset = 0;

    for (int i = 0; i < imDrawData->CmdListsCount; i++)
    {
        const ImDrawList* cmd_list = imDrawData->CmdLists[i];

        size_t vertexSize = cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
        vertexScratchBuffer->update(context, vertexOffset, vertexSize, cmd_list->VtxBuffer.Data);
        vertexOffset += vertexSize;

        size_t indexSize = cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
        indexScratchBuffer->update(context, indexOffset, indexSize, cmd_list->IdxBuffer.Data);
        indexOffset += indexSize;
    }

    return Result::Continue;
}
} // namespace renderer
