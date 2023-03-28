#pragma once

#include "rhi/resources.h"
#include "vulkan/core.h"

namespace vk
{
enum ResourceAccess
{
    Unused,
    ReadOnly,
    Write,
};

// This defines enum for VkPipelineStageFlagBits so that we can use it to compare and index into
// array.
enum class PipelineStage : uint16_t
{
    // Bellow are ordered based on Graphics Pipeline Stages
    TopOfPipe = 0,
    DrawIndirect = 1,
    VertexInput = 2,
    VertexShader = 3,
    GeometryShader = 4,
    TransformFeedback = 5,
    EarlyFragmentTest = 6,
    FragmentShader = 7,
    LateFragmentTest = 8,
    ColorAttachmentOutput = 9,

    // Compute specific pipeline Stage
    ComputeShader = 10,

    // Transfer specific pipeline Stage
    Transfer = 11,
    BottomOfPipe = 12,

    // Host specific pipeline stage
    Host = 13,

    InvalidEnum = 14,
    EnumCount = InvalidEnum,
};

struct ImageMemoryBarrierData
{
    char name[44];

    // The Vk layout corresponding to the ImageLayout key.
    VkImageLayout layout;

    // The stage in which the image is used (or Bottom/Top if not using any specific stage).  Unless
    // Bottom/Top (Bottom used for transition to and Top used for transition from), the two values
    // should match.
    VkPipelineStageFlags dstStageMask;
    VkPipelineStageFlags srcStageMask;
    // Access mask when transitioning into this layout.
    VkAccessFlags dstAccessMask;
    // Access mask when transitioning out from this layout.  Note that source access mask never
    // needs a READ bit, as WAR hazards don't need memory barriers (just execution barriers).
    VkAccessFlags srcAccessMask;
    // Read or write.
    ResourceAccess type;
    // CommandBufferHelper tracks an array of PipelineBarriers. This indicates which array element
    // this should be merged into. Right now we track individual barrier for every PipelineStage. If
    // layout has a single stage mask bit, we use that stage as index. If layout has multiple stage
    // mask bits, we pick the lowest stage as the index since it is the first stage that needs
    // barrier.
    PipelineStage barrierIndex;
};

constexpr VkPipelineStageFlags kPreFragmentStageFlags =
    VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
    VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;

constexpr VkPipelineStageFlags kAllShadersPipelineStageFlags =
    kPreFragmentStageFlags | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

constexpr VkPipelineStageFlags kAllDepthStencilPipelineStageFlags =
    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

// clang-format off
static std::map<rhi::ImageLayout, ImageMemoryBarrierData> kImageMemoryBarrierData = {
    {
        rhi::ImageLayout::Undefined,
        ImageMemoryBarrierData{
            "Undefined",
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            // Transition to: we don't expect to transition into Undefined.
            0,
            // Transition from: there's no data in the image to care about.
            0,
            ResourceAccess::ReadOnly,
            PipelineStage::InvalidEnum,
        },
    },
    {
        rhi::ImageLayout::ColorAttachment,
        ImageMemoryBarrierData{
            "ColorAttachment",
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            // Transition to: all reads and writes must happen after barrier.
            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            // Transition from: all writes must finish before barrier.
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            ResourceAccess::Write,
            PipelineStage::ColorAttachmentOutput,
        },
    },
    {
        rhi::ImageLayout::ColorAttachmentAndFragmentShaderRead,
        ImageMemoryBarrierData{
            "ColorAttachmentAndFragmentShaderRead",
            VK_IMAGE_LAYOUT_GENERAL,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            // Transition to: all reads and writes must happen after barrier.
            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT,
            // Transition from: all writes must finish before barrier.
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            ResourceAccess::Write,
            PipelineStage::FragmentShader,
        },
    },
    {
        rhi::ImageLayout::ColorAttachmentAndAllShadersRead,
        ImageMemoryBarrierData{
            "ColorAttachmentAndAllShadersRead",
            VK_IMAGE_LAYOUT_GENERAL,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | kAllShadersPipelineStageFlags,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | kAllShadersPipelineStageFlags,
            // Transition to: all reads and writes must happen after barrier.
            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT,
            // Transition from: all writes must finish before barrier.
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            ResourceAccess::Write,
            // In case of multiple destination stages, We barrier the earliest stage
            PipelineStage::VertexShader,
        },
    },
    {
        rhi::ImageLayout::DSAttachmentWriteAndFragmentShaderRead,
        ImageMemoryBarrierData{
            "DSAttachmentWriteAndFragmentShaderRead",
            VK_IMAGE_LAYOUT_GENERAL,
            kAllDepthStencilPipelineStageFlags | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            kAllDepthStencilPipelineStageFlags | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            // Transition to: all reads and writes must happen after barrier.
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT,
            // Transition from: all writes must finish before barrier.
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            ResourceAccess::Write,
            PipelineStage::FragmentShader,
        },
    },
    {
        rhi::ImageLayout::DSAttachmentWriteAndAllShadersRead,
        ImageMemoryBarrierData{
            "DSAttachmentWriteAndAllShadersRead",
            VK_IMAGE_LAYOUT_GENERAL,
            kAllDepthStencilPipelineStageFlags | kAllShadersPipelineStageFlags,
            kAllDepthStencilPipelineStageFlags | kAllShadersPipelineStageFlags,
            // Transition to: all reads and writes must happen after barrier.
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT,
            // Transition from: all writes must finish before barrier.
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            ResourceAccess::Write,
            // In case of multiple destination stages, We barrier the earliest stage
            PipelineStage::VertexShader,
        },
    },
    {
        rhi::ImageLayout::DSAttachmentReadAndFragmentShaderRead,
            ImageMemoryBarrierData{
            "DSAttachmentReadAndFragmentShaderRead",
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | kAllDepthStencilPipelineStageFlags,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | kAllDepthStencilPipelineStageFlags,
            // Transition to: all reads must happen after barrier.
            VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
            // Transition from: RAR and WAR don't need memory barrier.
            0,
            ResourceAccess::ReadOnly,
            PipelineStage::EarlyFragmentTest,
        },
    },
    {
        rhi::ImageLayout::DSAttachmentReadAndAllShadersRead,
            ImageMemoryBarrierData{
            "DSAttachmentReadAndAllShadersRead",
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            kAllShadersPipelineStageFlags | kAllDepthStencilPipelineStageFlags,
            kAllShadersPipelineStageFlags | kAllDepthStencilPipelineStageFlags,
            // Transition to: all reads must happen after barrier.
            VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
            // Transition from: RAR and WAR don't need memory barrier.
            0,
            ResourceAccess::ReadOnly,
            PipelineStage::VertexShader,
        },
    },
    {
        rhi::ImageLayout::DepthStencilAttachmentReadOnly,
            ImageMemoryBarrierData{
            "DepthStencilAttachmentReadOnly",
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            kAllDepthStencilPipelineStageFlags,
            kAllDepthStencilPipelineStageFlags,
            // Transition to: all reads must happen after barrier.
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
            // Transition from: RAR and WAR don't need memory barrier.
            0,
            ResourceAccess::ReadOnly,
            PipelineStage::EarlyFragmentTest,
        },
    },
    {
        rhi::ImageLayout::DepthStencilAttachment,
        ImageMemoryBarrierData{
            "DepthStencilAttachment",
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            kAllDepthStencilPipelineStageFlags,
            kAllDepthStencilPipelineStageFlags,
            // Transition to: all reads and writes must happen after barrier.
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            // Transition from: all writes must finish before barrier.
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            ResourceAccess::Write,
            PipelineStage::EarlyFragmentTest,
        },
    },
    {
        rhi::ImageLayout::DepthStencilResolveAttachment,
        ImageMemoryBarrierData{
            "DepthStencilResolveAttachment",
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            // Note: depth/stencil resolve uses color output stage and mask!
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            // Transition to: all reads and writes must happen after barrier.
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            // Transition from: all writes must finish before barrier.
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            ResourceAccess::Write,
            PipelineStage::ColorAttachmentOutput,
        },
    },
    {
        rhi::ImageLayout::Present,
        ImageMemoryBarrierData{
            "Present",
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            // transition to: vkQueuePresentKHR automatically performs the appropriate memory barriers:
            //
            // > Any writes to memory backing the images referenced by the pImageIndices and
            // > pSwapchains members of pPresentInfo, that are available before vkQueuePresentKHR
            // > is executed, are automatically made visible to the read access performed by the
            // > presentation engine.
            0,
            // Transition from: RAR and WAR don't need memory barrier.
            0,
            ResourceAccess::ReadOnly,
            PipelineStage::BottomOfPipe,
        },
    },
    {
        rhi::ImageLayout::SharedPresent,
        ImageMemoryBarrierData{
            "SharedPresent",
            VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            // Transition to: all reads and writes must happen after barrier.
            VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
            // Transition from: all writes must finish before barrier.
            VK_ACCESS_MEMORY_WRITE_BIT,
            ResourceAccess::Write,
            PipelineStage::BottomOfPipe,
        },
    },
    {
        rhi::ImageLayout::ExternalPreInitialized,
        ImageMemoryBarrierData{
            "ExternalPreInitialized",
            VK_IMAGE_LAYOUT_PREINITIALIZED,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_HOST_BIT | VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            // Transition to: we don't expect to transition into PreInitialized.
            0,
            // Transition from: all writes must finish before barrier.
            VK_ACCESS_MEMORY_WRITE_BIT,
            ResourceAccess::ReadOnly,
            PipelineStage::InvalidEnum,
        },
    },
    {
        rhi::ImageLayout::ExternalShadersReadOnly,
        ImageMemoryBarrierData{
            "ExternalShadersReadOnly",
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            // Transition to: all reads must happen after barrier.
            VK_ACCESS_SHADER_READ_BIT,
            // Transition from: RAR and WAR don't need memory barrier.
            0,
            ResourceAccess::ReadOnly,
            // In case of multiple destination stages, We barrier the earliest stage
            PipelineStage::TopOfPipe,
        },
    },
    {
        rhi::ImageLayout::ExternalShadersWrite,
        ImageMemoryBarrierData{
            "ExternalShadersWrite",
            VK_IMAGE_LAYOUT_GENERAL,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            // Transition to: all reads and writes must happen after barrier.
            VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
            // Transition from: all writes must finish before barrier.
            VK_ACCESS_SHADER_WRITE_BIT,
            ResourceAccess::Write,
            // In case of multiple destination stages, We barrier the earliest stage
            PipelineStage::TopOfPipe,
        },
    },
    {
        rhi::ImageLayout::TransferSrc,
        ImageMemoryBarrierData{
            "TransferSrc",
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            // Transition to: all reads must happen after barrier.
            VK_ACCESS_TRANSFER_READ_BIT,
            // Transition from: RAR and WAR don't need memory barrier.
            0,
            ResourceAccess::ReadOnly,
            PipelineStage::Transfer,
        },
    },
    {
        rhi::ImageLayout::TransferDst,
        ImageMemoryBarrierData{
            "TransferDst",
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            // Transition to: all writes must happen after barrier.
            VK_ACCESS_TRANSFER_WRITE_BIT,
            // Transition from: all writes must finish before barrier.
            VK_ACCESS_TRANSFER_WRITE_BIT,
            ResourceAccess::Write,
            PipelineStage::Transfer,
        },
    },
    {
        rhi::ImageLayout::VertexShaderReadOnly,
        ImageMemoryBarrierData{
            "VertexShaderReadOnly",
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
            VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
            // Transition to: all reads must happen after barrier.
            VK_ACCESS_SHADER_READ_BIT,
            // Transition from: RAR and WAR don't need memory barrier.
            0,
            ResourceAccess::ReadOnly,
            PipelineStage::VertexShader,
        },
    },
    {
        rhi::ImageLayout::VertexShaderWrite,
        ImageMemoryBarrierData{
            "VertexShaderWrite",
            VK_IMAGE_LAYOUT_GENERAL,
            VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
            VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
            // Transition to: all reads and writes must happen after barrier.
            VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
            // Transition from: all writes must finish before barrier.
            VK_ACCESS_SHADER_WRITE_BIT,
            ResourceAccess::Write,
            PipelineStage::VertexShader,
        },
    },
    {
        rhi::ImageLayout::PreFragmentShadersReadOnly,
        ImageMemoryBarrierData{
            "PreFragmentShadersReadOnly",
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            kPreFragmentStageFlags,
            kPreFragmentStageFlags,
            // Transition to: all reads must happen after barrier.
            VK_ACCESS_SHADER_READ_BIT,
            // Transition from: RAR and WAR don't need memory barrier.
            0,
            ResourceAccess::ReadOnly,
            // In case of multiple destination stages, We barrier the earliest stage
            PipelineStage::VertexShader,
        },
    },
    {
        rhi::ImageLayout::PreFragmentShadersWrite,
        ImageMemoryBarrierData{
            "PreFragmentShadersWrite",
            VK_IMAGE_LAYOUT_GENERAL,
            kPreFragmentStageFlags,
            kPreFragmentStageFlags,
            // Transition to: all reads and writes must happen after barrier.
            VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
            // Transition from: all writes must finish before barrier.
            VK_ACCESS_SHADER_WRITE_BIT,
            ResourceAccess::Write,
            // In case of multiple destination stages, We barrier the earliest stage
            PipelineStage::VertexShader,
        },
    },
    {
        rhi::ImageLayout::FragmentShaderReadOnly,
        ImageMemoryBarrierData{
            "FragmentShaderReadOnly",
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            // Transition to: all reads must happen after barrier.
            VK_ACCESS_SHADER_READ_BIT,
            // Transition from: RAR and WAR don't need memory barrier.
            0,
            ResourceAccess::ReadOnly,
            PipelineStage::FragmentShader,
        },
    },
    {
        rhi::ImageLayout::FragmentShaderWrite,
        ImageMemoryBarrierData{
            "FragmentShaderWrite",
            VK_IMAGE_LAYOUT_GENERAL,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            // Transition to: all reads and writes must happen after barrier.
            VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
            // Transition from: all writes must finish before barrier.
            VK_ACCESS_SHADER_WRITE_BIT,
            ResourceAccess::Write,
            PipelineStage::FragmentShader,
        },
    },
    {
        rhi::ImageLayout::ComputeShaderReadOnly,
        ImageMemoryBarrierData{
            "ComputeShaderReadOnly",
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            // Transition to: all reads must happen after barrier.
            VK_ACCESS_SHADER_READ_BIT,
            // Transition from: RAR and WAR don't need memory barrier.
            0,
            ResourceAccess::ReadOnly,
            PipelineStage::ComputeShader,
        },
    },
    {
        rhi::ImageLayout::ComputeShaderWrite,
        ImageMemoryBarrierData{
            "ComputeShaderWrite",
            VK_IMAGE_LAYOUT_GENERAL,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            // Transition to: all reads and writes must happen after barrier.
            VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
            // Transition from: all writes must finish before barrier.
            VK_ACCESS_SHADER_WRITE_BIT,
            ResourceAccess::Write,
            PipelineStage::ComputeShader,
        },
    },
    {
        rhi::ImageLayout::AllGraphicsShadersReadOnly,
        ImageMemoryBarrierData{
            "AllGraphicsShadersReadOnly",
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            kAllShadersPipelineStageFlags,
            kAllShadersPipelineStageFlags,
            // Transition to: all reads must happen after barrier.
            VK_ACCESS_SHADER_READ_BIT,
            // Transition from: RAR and WAR don't need memory barrier.
            0,
            ResourceAccess::ReadOnly,
            // In case of multiple destination stages, We barrier the earliest stage
            PipelineStage::VertexShader,
        },
    },
    {
        rhi::ImageLayout::AllGraphicsShadersWrite,
        ImageMemoryBarrierData{
            "AllGraphicsShadersWrite",
            VK_IMAGE_LAYOUT_GENERAL,
            kAllShadersPipelineStageFlags,
            kAllShadersPipelineStageFlags,
            // Transition to: all reads and writes must happen after barrier.
            VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
            // Transition from: all writes must finish before barrier.
            VK_ACCESS_SHADER_WRITE_BIT,
            ResourceAccess::Write,
            // In case of multiple destination stages, We barrier the earliest stage
            PipelineStage::VertexShader,
        },
    },
};

inline VkImageLayout convertImageLayout(rhi::ImageLayout imageLayout)
{
    return kImageMemoryBarrierData[imageLayout].layout;
}

inline std::string getImageLayoutName(rhi::ImageLayout imageLayout)
{
    return kImageMemoryBarrierData[imageLayout].name;
}
}