#pragma once

namespace rhi
{
class GraphicsPipelineState
{
  public:
    GraphicsPipelineState()
    {
    }

  private:
    /*
    - shader
    - vertex input
    - input assembly (topology, ...)
        VkPrimitiveTopology                        topology;
      VkBool32                                   primitiveRestartEnable;

    - tesellation state
    uint32_t                                  patchControlPoints;

    - viewport state (viewport , scissor)
        uint32_t                              viewportCount;
      const VkViewport*                     pViewports;
      uint32_t                              scissorCount;
      const VkRect2D*                       pScissors;

    - rasterization (polygon mode, line width, cull, frontface, ...)
        VkBool32                                   depthClampEnable;
      VkBool32                                   rasterizerDiscardEnable;
      VkPolygonMode                              polygonMode;
      VkCullModeFlags                            cullMode;
      VkFrontFace                                frontFace;
      VkBool32                                   depthBiasEnable;
      float                                      depthBiasConstantFactor;
      float                                      depthBiasClamp;
      float                                      depthBiasSlopeFactor;
      float                                      lineWidth;

    - multisample state
        VkSampleCountFlagBits                    rasterizationSamples;
      VkBool32                                 sampleShadingEnable;
      float                                    minSampleShading;
      const VkSampleMask*                      pSampleMask;
      VkBool32                                 alphaToCoverageEnable;
      VkBool32                                 alphaToOneEnable;

      - depth state
      VkBool32                                  depthTestEnable;
      VkBool32                                  depthWriteEnable;
      VkCompareOp                               depthCompareOp;
      VkBool32                                  depthBoundsTestEnable;
      VkBool32                                  stencilTestEnable;
      VkStencilOpState                          front;
      VkStencilOpState                          back;
      float                                     minDepthBounds;
      float                                     maxDepthBounds;

      - stencil state
          VkStencilOp    failOp;
      VkStencilOp    passOp;
      VkStencilOp    depthFailOp;
      VkCompareOp    compareOp;
      uint32_t       compareMask;
      uint32_t       writeMask;
      uint32_t       reference;

    - color blend (needs rt infos)
        VkBool32                 blendEnable;
      VkBlendFactor            srcColorBlendFactor;
      VkBlendFactor            dstColorBlendFactor;
      VkBlendOp                colorBlendOp;
      VkBlendFactor            srcAlphaBlendFactor;
      VkBlendFactor            dstAlphaBlendFactor;
      VkBlendOp                alphaBlendOp;
      VkColorComponentFlags    colorWriteMask;
      X
      kBool32                                      logicOpEnable;
      VkLogicOp                                     logicOp;
      uint32_t                                      attachmentCount;
      const VkPipelineColorBlendAttachmentState*    pAttachments;
      float                                         blendConstants[4];

    * pipeline layout (push constants) <> descriptorset layout
    */
};
} // namespace rhi