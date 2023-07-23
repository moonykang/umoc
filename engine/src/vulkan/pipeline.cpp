#include "pipeline.h"
#include "commandBuffer.h"
#include "common/hash.h"
#include "context.h"
#include "descriptor.h"
#include "device.h"
#include "pendingState.h"
#include "renderpass.h"
#include "rendertarget.h"
#include "resources.h"

namespace vk
{

rhi::VertexShaderBase* Context::createVertexShader(rhi::ResourceID id, std::string name,
                                                   rhi::VertexChannelFlags vertexChannelFlags)
{
    return new VertexShader(id, name, vertexChannelFlags);
}

rhi::VertexShaderBase* Context::createVertexShader(rhi::ResourceID id, std::string name,
                                                   std::vector<rhi::VertexAttribute>& vertexAttribute, uint32_t stride)
{
    return new VertexShader(id, name, vertexAttribute, stride);
}

rhi::PixelShaderBase* Context::createPixelShader(rhi::ResourceID id, std::string name)
{
    return new FragmentShader(id, name);
}

rhi::ComputeShaderBase* Context::createComputeShader(rhi::ResourceID id, std::string name)
{
    return new ComputeShader(id, name);
}

VertexShader::VertexShader(rhi::ResourceID id, std::string name, rhi::VertexChannelFlags vertexChannelFlags)
    : rhi::VertexShaderBase(id, name), Shader(), stride(sizeof(rhi::Vertex))
{
    generateInputAttributeDescriptions(vertexChannelFlags);
}

VertexShader::VertexShader(rhi::ResourceID id, std::string name, std::vector<rhi::VertexAttribute>& vertexAttribute,
                           uint32_t stride)
    : rhi::VertexShaderBase(id, name), Shader(), stride(stride)
{
    generateInputAttributeDescriptions(vertexAttribute);
}

Result VertexShader::initRHI(rhi::Context* rhiContext)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);
    return Shader::init(context, this);
}

void VertexShader::terminateRHI(rhi::Context* rhiContext)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);
    Shader::terminate(context->getDevice()->getHandle());
}

void VertexShader::generateInputAttributeDescriptions(rhi::VertexChannelFlags vertexChannelFlags)
{
    uint32_t location = 0;
    if ((vertexChannelFlags & rhi::VertexChannel::Position) != 0)
    {
        vertexInputAttributeDescriptions.push_back(
            {location++, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(rhi::Vertex, position)});
    }

    if ((vertexChannelFlags & rhi::VertexChannel::Normal) != 0)
    {
        vertexInputAttributeDescriptions.push_back(
            {location++, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(rhi::Vertex, normal)});
    }

    if ((vertexChannelFlags & rhi::VertexChannel::Uv) != 0)
    {
        vertexInputAttributeDescriptions.push_back({location++, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(rhi::Vertex, uv)});
    }

    if ((vertexChannelFlags & rhi::VertexChannel::Color) != 0)
    {
        vertexInputAttributeDescriptions.push_back(
            {location++, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(rhi::Vertex, color)});
    }

    if ((vertexChannelFlags & rhi::VertexChannel::Tangent) != 0)
    {
        vertexInputAttributeDescriptions.push_back(
            {location++, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(rhi::Vertex, tangent)});
    }

    if ((vertexChannelFlags & rhi::VertexChannel::Bitangent) != 0)
    {
        vertexInputAttributeDescriptions.push_back(
            {location++, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(rhi::Vertex, tangent)});
    }

    if ((vertexChannelFlags & rhi::VertexChannel::Joint0) != 0)
    {
        vertexInputAttributeDescriptions.push_back(
            {location++, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(rhi::Vertex, joint0)});
    }

    if ((vertexChannelFlags & rhi::VertexChannel::Weight0) != 0)
    {
        vertexInputAttributeDescriptions.push_back(
            {location++, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(rhi::Vertex, weight0)});
    }
}

void VertexShader::generateInputAttributeDescriptions(std::vector<rhi::VertexAttribute>& vertexAttributes)
{
    uint32_t location = 0;
    for (auto vertexAttribute : vertexAttributes)
    {
        VkFormat format = kFormatMap[vertexAttribute.format].format;
        uint32_t offset = vertexAttribute.offset;

        vertexInputAttributeDescriptions.push_back({location++, 0, format, offset});
    }
}

std::vector<VkVertexInputAttributeDescription>& VertexShader::getVertexInputAttributes()
{
    return vertexInputAttributeDescriptions;
}

uint32_t VertexShader::getStride()
{
    return stride;
}

FragmentShader::FragmentShader(rhi::ResourceID id, std::string name) : rhi::PixelShaderBase(id, name), Shader()
{
}

Result FragmentShader::initRHI(rhi::Context* rhiContext)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);
    return Shader::init(context, this);
}

void FragmentShader::terminateRHI(rhi::Context* rhiContext)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);
    Shader::terminate(context->getDevice()->getHandle());
}

ComputeShader::ComputeShader(rhi::ResourceID id, std::string name) : rhi::ComputeShaderBase(id, name), Shader()
{
}

Result ComputeShader::initRHI(rhi::Context* rhiContext)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);
    return Shader::init(context, this);
}

void ComputeShader::terminateRHI(rhi::Context* rhiContext)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);
    Shader::terminate(context->getDevice()->getHandle());
}

Shader::Shader() : pipelineShaderStageCreateInfo({})
{
}

Result Shader::init(Context* context, rhi::ShaderBase* shaderBase)
{
    ASSERT(!valid());

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderBase->size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderBase->data());

    vk_try(vkCreateShaderModule(context->getDevice()->getHandle(), &createInfo, nullptr, &mHandle));

    pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineShaderStageCreateInfo.stage =
        static_cast<VkShaderStageFlagBits>(convertToVkShaderStage(shaderBase->getShaderStage()));
    pipelineShaderStageCreateInfo.module = mHandle;
    pipelineShaderStageCreateInfo.pName = "main";

    return Result::Continue;
}

void Shader::terminate(VkDevice device)
{
    if (valid())
    {
        vkDestroyShaderModule(device, mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }
}

// TODO: empty layout now
Result PipelineLayout::init(Context* context, std::vector<VkDescriptorSetLayout>& descriptorSetLayouts,
                            std::vector<rhi::PushConstant>& rhiPushConstants)
{
    ASSERT(!valid());

    std::vector<VkPushConstantRange> pushConstants;

    for (auto& rhiPushConstant : rhiPushConstants)
    {
        auto& pushConstant = pushConstants.emplace_back();
        pushConstant.stageFlags = convertToVkShaderStage(rhiPushConstant.shaderStageFlags);
        pushConstant.offset = rhiPushConstant.offset;
        pushConstant.size = rhiPushConstant.size;
    }

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
    pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();

    vk_try(vkCreatePipelineLayout(context->getDevice()->getHandle(), &pipelineLayoutInfo, nullptr, &mHandle));

    return Result::Continue;
}

void PipelineLayout::terminate(VkDevice device)
{
    if (valid())
    {
        vkDestroyPipelineLayout(device, mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }
}

class PipelineHashStruct
{
  public:
    PipelineHashStruct()
    {
        memset(this, 0, sizeof(PipelineHashStruct));
    }

    size_t getHash()
    {
        return util::computeGenericHash(this, sizeof(PipelineHashStruct));
    }

    size_t pipelineStateHash;
    size_t vertexShaderHash;
    size_t pixelShaderHash;
    size_t renderpassHash;
};

class ComputePipelineHashStruct
{
  public:
    ComputePipelineHashStruct()
    {
        memset(this, 0, sizeof(ComputePipelineHashStruct));
    }

    size_t getHash()
    {
        return util::computeGenericHash(this, sizeof(ComputePipelineHashStruct));
    }

    size_t pipelineStateHash;
    size_t computeShaderHash;
};

void PipelineCache::terminate(VkDevice device)
{
    if (valid())
    {
        vkDestroyPipelineCache(device, mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }
}

VkResult PipelineCache::init(VkDevice device, const VkPipelineCacheCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreatePipelineCache(device, &createInfo, nullptr, &mHandle);
}

VkResult PipelineCache::getCacheData(VkDevice device, size_t* pCacheSize, void* cacheData)
{
    ASSERT(!valid());
    return vkGetPipelineCacheData(device, mHandle, pCacheSize, cacheData);
}

VkResult PipelineCache::merge(VkDevice device, uint32_t srcCacheCount, const VkPipelineCache* pSrcCaches)
{
    ASSERT(!valid());
    return vkMergePipelineCaches(device, mHandle, srcCacheCount, pSrcCaches);
}

Result Context::createGfxPipeline(rhi::GraphicsPipelineState gfxPipelineState)
{
    Pipeline* pipeline = pipelineMap->getPipeline(this, gfxPipelineState);

    if (!pipeline)
    {
        return Result::Fail;
    }

    pendingState->setPipeline(pipeline);

    return Result::Continue;
}

Result Context::createComputePipeline(rhi::ComputePipelineState pipelineState)
{
    Pipeline* pipeline = pipelineMap->getPipeline(this, pipelineState);

    if (!pipeline)
    {
        return Result::Fail;
    }

    pendingState->setPipeline(pipeline);

    return Result::Continue;
}

void Context::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    CommandBuffer* commandBuffer = getActiveCommandBuffer();
    Pipeline* bindPipeline = pendingState->getPipeline();
    bindPipeline->bind(commandBuffer);
    commandBuffer->draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void Context::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset,
                          uint32_t firstInstance)
{
    CommandBuffer* commandBuffer = getActiveCommandBuffer();
    Pipeline* bindPipeline = pendingState->getPipeline();
    bindPipeline->bind(commandBuffer);
    commandBuffer->drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void Context::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    CommandBuffer* commandBuffer = getActiveCommandBuffer();
    Pipeline* bindPipeline = pendingState->getPipeline();
    bindPipeline->bind(commandBuffer);
    commandBuffer->dispatch(groupCountX, groupCountY, groupCountZ);
}

void Context::pushConstant(rhi::ShaderStageFlags shaderStage, size_t size, void* data)
{
    Pipeline* bindPipeline = pendingState->getPipeline();
    getActiveCommandBuffer()->pushConstants(bindPipeline->getLayout()->getHandle(), shaderStage, size, data);
}

PipelineMap::PipelineMap() : pipelineCache(nullptr)
{
}

Result PipelineMap::init(Context* context)
{
    pipelineCache = new PipelineCache();

    return Result::Continue;
}

void PipelineMap::terminate(VkDevice device)
{
    for (auto& pipeline : pipelineMap)
    {
        TERMINATE(pipeline.second, device);
    }
    pipelineMap.clear();

    TERMINATE(pipelineCache, device);
}

Pipeline* PipelineMap::getPipeline(Context* context, rhi::ComputePipelineState& pipelineState)
{
    ComputePipelineHashStruct pipelineHashStruct;
    pipelineHashStruct.pipelineStateHash = pipelineState.getHash();
    pipelineHashStruct.computeShaderHash = pipelineState.shaderParameters->computeShader->getID();

    size_t pipelineHash = pipelineHashStruct.getHash();

    if (auto search = pipelineMap.find(pipelineHash); search != pipelineMap.end())
    {
        return search->second;
    }

    try
    {
        ComputeShader* computeShader = reinterpret_cast<ComputeShader*>(pipelineState.shaderParameters->computeShader);

        VkPipelineShaderStageCreateInfo shaderStageCreateInfo = computeShader->getPipelineShaderStageCreateInfo();

        auto rhiDescriptorSets = pipelineState.shaderParameters->getDescriptorSets();
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
        for (auto rhiDescriptorSet : rhiDescriptorSets)
        {
            DescriptorSet* descriptorSet = reinterpret_cast<DescriptorSet*>(rhiDescriptorSet);
            descriptorSetLayouts.push_back(descriptorSet->getLayout()->getHandle());
        }

        if (descriptorSetLayouts.empty())
        {
            descriptorSetLayouts.push_back(context->getEmptyDescriptorSetLayout()->getHandle());
        }

        Pipeline* newPipeline = new Pipeline(VK_PIPELINE_BIND_POINT_COMPUTE);
        newPipeline->init(context);
        newPipeline->getLayout()->init(context, descriptorSetLayouts, pipelineState.pushConstants);

        VkComputePipelineCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.stage = shaderStageCreateInfo;
        createInfo.layout = newPipeline->getLayout()->getHandle();
        createInfo.basePipelineHandle = VK_NULL_HANDLE;

        call(newPipeline->createCompute(context->getDevice()->getHandle(), createInfo, nullptr));

        computePipelineMap.insert({pipelineHash, newPipeline});

        return newPipeline;
    }
    catch (std::exception& e)
    {
        LOGE("Exception %s", e.what());
    }

    return nullptr;
}

Pipeline* PipelineMap::getPipeline(Context* context, rhi::GraphicsPipelineState& gfxPipelineState)
{
    PipelineHashStruct pipelineHashStruct;
    pipelineHashStruct.pipelineStateHash = gfxPipelineState.getHash();
    // TODO
    pipelineHashStruct.vertexShaderHash = gfxPipelineState.shaderParameters->vertexShader->getID();
    pipelineHashStruct.pixelShaderHash = gfxPipelineState.shaderParameters->pixelShader->getID();
    pipelineHashStruct.renderpassHash = context->getCurrentRenderpassHash();

    size_t pipelineHash = pipelineHashStruct.getHash();

    // LOGD("pipelineHash %zu pipelineMap size %zu", pipelineHash, pipelineMap.size());

    if (auto search = pipelineMap.find(pipelineHash); search != pipelineMap.end())
    {
        return search->second;
    }

    try
    {
        VertexShader* vertexShader = reinterpret_cast<VertexShader*>(gfxPipelineState.shaderParameters->vertexShader);
        FragmentShader* pixelShader = reinterpret_cast<FragmentShader*>(gfxPipelineState.shaderParameters->pixelShader);

        std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfos;
        shaderStageInfos.push_back(vertexShader->getPipelineShaderStageCreateInfo());
        shaderStageInfos.push_back(pixelShader->getPipelineShaderStageCreateInfo());

        VkVertexInputBindingDescription vertexInputBindingDescription;
        vertexInputBindingDescription.binding = 0;
        vertexInputBindingDescription.stride = vertexShader->getStride();
        vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions =
            vertexShader->getVertexInputAttributes();

        VkPipelineVertexInputStateCreateInfo vertexInputState = {};
        vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputState.vertexBindingDescriptionCount = 1;
        vertexInputState.pVertexBindingDescriptions = &vertexInputBindingDescription;
        vertexInputState.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(vertexInputAttributeDescriptions.size());
        vertexInputState.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
        inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyState.topology = convertToVkPrimitiveTopology(gfxPipelineState.assemblyState.primitiveTopology);
        inputAssemblyState.primitiveRestartEnable = gfxPipelineState.assemblyState.primitiveRestartEnable;

        VkPipelineRasterizationStateCreateInfo rasterizationState = {};
        rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationState.depthClampEnable = gfxPipelineState.rasterizationState.depthClampEnable;
        rasterizationState.rasterizerDiscardEnable = gfxPipelineState.rasterizationState.rasterizerDiscardEnable;
        rasterizationState.polygonMode = convertToVkPolygonMode(gfxPipelineState.rasterizationState.polygonMode);
        rasterizationState.cullMode = convertToVkCullMode(gfxPipelineState.rasterizationState.cullMode);
        rasterizationState.frontFace = convertToVkFrontFace(gfxPipelineState.rasterizationState.frontFace);
        rasterizationState.depthBiasEnable = gfxPipelineState.rasterizationState.depthBiasEnable;
        rasterizationState.depthBiasConstantFactor = gfxPipelineState.rasterizationState.depthBiasConstantFactor;
        rasterizationState.depthBiasClamp = gfxPipelineState.rasterizationState.depthBiasClamp;
        rasterizationState.depthBiasSlopeFactor = gfxPipelineState.rasterizationState.depthBiasSlopeFactor;
        rasterizationState.lineWidth = gfxPipelineState.rasterizationState.lineWidth;

        VkPipelineTessellationStateCreateInfo tessellationState = {};
        tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        tessellationState.pNext = nullptr;
        tessellationState.patchControlPoints = gfxPipelineState.tessellationState.patchControlPoints;

        VkPipelineMultisampleStateCreateInfo multisampleState = {};
        multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleState.rasterizationSamples =
            convertToVkSampleCountBits(gfxPipelineState.multisampleState.sampleCount);
        multisampleState.sampleShadingEnable = gfxPipelineState.multisampleState.sampleShadingEnable;
        multisampleState.minSampleShading = gfxPipelineState.multisampleState.minSampleShading;
        multisampleState.alphaToCoverageEnable = gfxPipelineState.multisampleState.alphaToCoverageEnable;
        multisampleState.alphaToOneEnable = gfxPipelineState.multisampleState.alphaToOneEnable;
        multisampleState.pSampleMask = nullptr;

        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        auto convertStencilState = [](rhi::StencilOpState rhiStencilOpState) {
            VkStencilOpState stencilOpState;
            stencilOpState.failOp = convertToVkStencilOp(rhiStencilOpState.failOp);
            stencilOpState.passOp = convertToVkStencilOp(rhiStencilOpState.passOp);
            stencilOpState.depthFailOp = convertToVkStencilOp(rhiStencilOpState.depthFailOp);
            stencilOpState.compareOp = convertToVkCompareOp(rhiStencilOpState.compareOp);
            stencilOpState.compareMask = rhiStencilOpState.compareMask;
            stencilOpState.writeMask = rhiStencilOpState.writeMask;
            stencilOpState.reference = rhiStencilOpState.reference;

            return stencilOpState;
        };

        VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilState = {};
        pipelineDepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        pipelineDepthStencilState.pNext = nullptr;
        pipelineDepthStencilState.flags = 0;
        pipelineDepthStencilState.depthTestEnable = gfxPipelineState.depthStencilState.depthTestEnable;
        pipelineDepthStencilState.depthWriteEnable = gfxPipelineState.depthStencilState.depthWriteEnable;
        pipelineDepthStencilState.depthCompareOp =
            convertToVkCompareOp(gfxPipelineState.depthStencilState.depthCompareOp);
        pipelineDepthStencilState.depthBoundsTestEnable = gfxPipelineState.depthStencilState.depthBoundsTestEnable;
        pipelineDepthStencilState.stencilTestEnable = gfxPipelineState.depthStencilState.stencilTestEnable;
        pipelineDepthStencilState.front = convertStencilState(gfxPipelineState.depthStencilState.front);
        pipelineDepthStencilState.back = convertStencilState(gfxPipelineState.depthStencilState.back);
        pipelineDepthStencilState.minDepthBounds = gfxPipelineState.depthStencilState.minDepthBounds;
        pipelineDepthStencilState.maxDepthBounds = gfxPipelineState.depthStencilState.maxDepthBounds;

        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates;
        for (uint32_t i = 0; i < gfxPipelineState.colorBlendState.attachmentCount; i++)
        {
            VkPipelineColorBlendAttachmentState& colorBlendAttachmentState = colorBlendAttachmentStates.emplace_back();
            colorBlendAttachmentState.blendEnable =
                gfxPipelineState.colorBlendState.colorBlendAttachmentStates[i].blendEnable;
            colorBlendAttachmentState.srcColorBlendFactor = convertToVkBlendFactor(
                gfxPipelineState.colorBlendState.colorBlendAttachmentStates[i].srcColorBlendFactor);
            colorBlendAttachmentState.dstColorBlendFactor = convertToVkBlendFactor(
                gfxPipelineState.colorBlendState.colorBlendAttachmentStates[i].dstColorBlendFactor);
            colorBlendAttachmentState.colorBlendOp =
                convertToVkBlendFOp(gfxPipelineState.colorBlendState.colorBlendAttachmentStates[i].colorBlendOp);
            colorBlendAttachmentState.srcAlphaBlendFactor = convertToVkBlendFactor(
                gfxPipelineState.colorBlendState.colorBlendAttachmentStates[i].srcAlphaBlendFactor);
            colorBlendAttachmentState.dstAlphaBlendFactor = convertToVkBlendFactor(
                gfxPipelineState.colorBlendState.colorBlendAttachmentStates[i].dstAlphaBlendFactor);
            colorBlendAttachmentState.alphaBlendOp =
                convertToVkBlendFOp(gfxPipelineState.colorBlendState.colorBlendAttachmentStates[i].alphaBlendOp);
            colorBlendAttachmentState.colorWriteMask = static_cast<VkColorComponentFlags>(
                gfxPipelineState.colorBlendState.colorBlendAttachmentStates[i].colorWriteMask);
        }

        VkPipelineColorBlendStateCreateInfo colorBlendState = {};
        colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendState.logicOpEnable = gfxPipelineState.colorBlendState.logicOpEnable;
        colorBlendState.logicOp = convertToVkLogicOp(gfxPipelineState.colorBlendState.logicOp);
        colorBlendState.attachmentCount = static_cast<uint32_t>(colorBlendAttachmentStates.size());
        colorBlendState.pAttachments = colorBlendAttachmentStates.data();
        colorBlendState.blendConstants[0] = gfxPipelineState.colorBlendState.blendConstants[0];
        colorBlendState.blendConstants[1] = gfxPipelineState.colorBlendState.blendConstants[1];
        colorBlendState.blendConstants[2] = gfxPipelineState.colorBlendState.blendConstants[2];
        colorBlendState.blendConstants[3] = gfxPipelineState.colorBlendState.blendConstants[3];

        std::vector<VkDynamicState> dynamicStateList;
        {
            dynamicStateList.push_back(VK_DYNAMIC_STATE_VIEWPORT);
            dynamicStateList.push_back(VK_DYNAMIC_STATE_SCISSOR);
        }

        VkPipelineDynamicStateCreateInfo dynamicState = {};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateList.size());
        dynamicState.pDynamicStates = dynamicStateList.data();

        auto rhiDescriptorSets = gfxPipelineState.shaderParameters->getDescriptorSets();
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
        for (auto rhiDescriptorSet : rhiDescriptorSets)
        {
            DescriptorSet* descriptorSet = reinterpret_cast<DescriptorSet*>(rhiDescriptorSet);
            descriptorSetLayouts.push_back(descriptorSet->getLayout()->getHandle());
        }

        if (descriptorSetLayouts.empty())
        {
            descriptorSetLayouts.push_back(context->getEmptyDescriptorSetLayout()->getHandle());
        }

        Pipeline* newPipeline = new Pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS);
        newPipeline->init(context);
        newPipeline->getLayout()->init(context, descriptorSetLayouts, gfxPipelineState.pushConstants);

        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
        graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStageInfos.size());
        graphicsPipelineCreateInfo.pStages = shaderStageInfos.data();
        graphicsPipelineCreateInfo.pVertexInputState = &vertexInputState;
        graphicsPipelineCreateInfo.pTessellationState = &tessellationState;
        graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
        graphicsPipelineCreateInfo.pViewportState = &viewportState;
        graphicsPipelineCreateInfo.pRasterizationState = &rasterizationState;
        graphicsPipelineCreateInfo.pMultisampleState = &multisampleState;
        graphicsPipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilState;
        graphicsPipelineCreateInfo.pColorBlendState = &colorBlendState;
        graphicsPipelineCreateInfo.pDynamicState = &dynamicState;
        graphicsPipelineCreateInfo.layout = newPipeline->getLayout()->getHandle();
        graphicsPipelineCreateInfo.renderPass = context->getCurrentRenderpass()->getHandle();
        graphicsPipelineCreateInfo.subpass = 0;
        graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

        call(newPipeline->createGraphics(context->getDevice()->getHandle(), graphicsPipelineCreateInfo, nullptr));

        pipelineMap.insert({pipelineHash, newPipeline});

        return newPipeline;
    }
    catch (std::exception& e)
    {
        LOGE("Exception %s", e.what());
    }

    return nullptr;
}

Pipeline::Pipeline(VkPipelineBindPoint pipelineBindPoint) : pipelineBindPoint(pipelineBindPoint), layout(nullptr)
{
}

Result Pipeline::init(Context* context)
{
    layout = new PipelineLayout();

    return Result::Continue;
}

void Pipeline::bind(CommandBuffer* commandBuffer)
{
    ASSERT(valid());
    commandBuffer->bindPipeline(pipelineBindPoint, mHandle);
}

void Pipeline::terminate(VkDevice device)
{
    TERMINATE(layout, device);
    vkDestroyPipeline(device, mHandle, nullptr);
    mHandle = VK_NULL_HANDLE;
}

VkResult Pipeline::createGraphics(VkDevice device, const VkGraphicsPipelineCreateInfo& createInfo,
                                  const VkPipelineCache& pipelineCache)
{
    ASSERT(!valid());

    VkResult result = vkCreateGraphicsPipelines(device, pipelineCache, 1, &createInfo, nullptr, &mHandle);
    CAPTURE_COMMAND("[vkCreateGraphicsPipelines] device: %p, pipeline: %p, result: %u", device, mHandle, result);
    return result;
}

VkResult Pipeline::createCompute(VkDevice device, const VkComputePipelineCreateInfo& createInfo,
                                 const VkPipelineCache& pipelineCache)
{
    ASSERT(!valid());
    VkResult result = vkCreateComputePipelines(device, pipelineCache, 1, &createInfo, nullptr, &mHandle);
    CAPTURE_COMMAND("[vkCreateComputePipelines] device: %p, pipeline: %p, result: %u", device, mHandle, result);
    return result;
}
} // namespace vk