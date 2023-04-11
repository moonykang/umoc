#include "ui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "model/vertexInput.h"
#include "rhi/context.h"
#include "rhi/defines.h"
#include "rhi/descriptor.h"
#include "rhi/image.h"
#include "rhi/shader.h"

class UIVertexShader : public rhi::VertexShaderBase
{
  public:
    UIVertexShader()
        : rhi::VertexShaderBase("ui.vert.spv",
                                rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Color)
    {
    }
};

class UIFragmentShader : public rhi::PixelShaderBase
{
  public:
    UIFragmentShader() : rhi::PixelShaderBase("ui.frag.spv")
    {
    }
};

struct UIPushBlock
{
    glm::vec2 scale;
    glm::vec2 translate;
} uiPushBlock;

class UIShaderParameters : public rhi::ShaderParameters
{
  public:
    UIShaderParameters() : ShaderParameters(), uiDescriptor(nullptr)
    {
    }

    std::vector<rhi::DescriptorSet*> getDescriptorSets() override
    {
        return {uiDescriptor};
    }

    rhi::DescriptorSet* uiDescriptor;
};

UIVertexShader uiVertexShader;
UIFragmentShader uiPixelShader;

namespace renderer
{
Result UIPass::init(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    vertexInput = new model::VertexInput();

    try(uiVertexShader.init(context));
    try(uiPixelShader.init(context));

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

    std::vector<char> data;
    for (uint32_t i; i < width * height * 4; i++)
    {
        data.push_back(fontData[i]);
    }

    fontTexture = new rhi::Texture("uiFont");
    try(fontTexture->init(context, rhi::Format::R8G8B8A8_UNORM,
                          {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1}, rhi::ImageUsage::SAMPLED,
                          fontSize, fontData));

    uiDescriptorSet = context->allocateDescriptorSet();

    std::vector<uint32_t> offsets = {0};
    rhi::DescriptorInfoList descriptorInfoList;
    rhi::DescriptorList descriptorList;

    descriptorInfoList.push_back({0, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler});
    descriptorList.push_back(
        {{0, rhi::ShaderStage::Pixel, rhi::DescriptorType::Combined_Image_Sampler}, fontTexture->getImageDescriptor()});

    try(uiDescriptorSet->init(context, descriptorInfoList));
    try(uiDescriptorSet->update(context, descriptorList, offsets));

    return Result::Continue;
}

void UIPass::terminate(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    TERMINATE(vertexInput, platformContext);
    TERMINATE(fontTexture, context);
    TERMINATE(uiDescriptorSet, context);
}

Result UIPass::render(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    try(updateUI());

    try(updateBuffers(platformContext));
    /*
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    UIShaderParameters params;
    params.vertexShader = &uiVertexShader;
    params.pixelShader = &uiPixelShader;
    params.uiDescriptor = uiDescriptorSet;

    rhi::GraphicsPipelineState graphicsPipelineState;
    graphicsPipelineState.shaderParameters = &params;
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

    graphicsPipelineState.pushConstants.push_back(
        rhi::PushConstant(rhi::ShaderStage::Vertex | rhi::ShaderStage::Pixel, 0, sizeof(UIPushBlock)));

    // context->pushConstant(rhi::ShaderStage::Vertex | rhi::ShaderStage::Pixel, sizeof(PushBlock), &pushBlock);
*/
    return Result::Continue;
}

Result UIPass::updateUI()
{
    ImGui::NewFrame();

    // Init imGui windows and elements
    // SRS - Set initial position of default Debug window (note: Debug window sets its own initial size, use
    // ImGuiSetCond_Always to override)
    ImGui::SetWindowPos(ImVec2(20, 20), ImGuiCond_::ImGuiCond_FirstUseEver);
    ImGui::SetWindowSize(ImVec2(300, 300), ImGuiCond_::ImGuiCond_Always);
    ImGui::TextUnformatted("Hello");
    ImGui::TextUnformatted("WTF");

    ImGui::Render();
    return Result::Continue;
}

Result UIPass::updateBuffers(platform::Context* context)
{
    ImDrawData* imDrawData = ImGui::GetDrawData();

    // Note: Alignment is done inside buffer creation
    VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
    VkDeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

    LOGD("indexBufferSize %zu", indexBufferSize);
    LOGD("vertexBufferSize %zu", vertexBufferSize);
    LOGD("imDrawData->CmdListsCount %zu", imDrawData->CmdListsCount);
    if ((vertexBufferSize == 0) || (indexBufferSize == 0))
    {
        Result::Continue;
    }
    return Result::Continue;
}
} // namespace renderer
