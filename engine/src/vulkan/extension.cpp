#include "vulkan/extension.h"
#include "context.h"
#include "device.h"
#include "instance.h"
#include "vulkan/core.h"

#if PLATFORM_MAC
#include <dlfcn.h>
#endif

namespace vk
{
bool Instance::supportExtension(ExtensionName extensionName)
{
    if (auto search = instanceExtensions.find(extensionName); search != instanceExtensions.end())
    {
        return search->second->isSupport();
    }
    return false;
}

bool Device::supportExtension(ExtensionName extensionName)
{
    if (auto search = deviceExtensions.find(extensionName); search != deviceExtensions.end())
    {
        return search->second->isSupport();
    }
    return false;
}

bool Context::supportInstanceExtension(ExtensionName extensionName)
{
    return instance->supportExtension(extensionName);
}

bool Context::supportDeviceExtension(ExtensionName extensionName)
{
    return device->supportExtension(extensionName);
}

#define GET_INSTANCE_PROC(instance, func) func = reinterpret_cast<PFN_##func>(vkGetInstanceProcAddr(instance, #func))
#define GET_DEVICE_PROC(device, func) func = reinterpret_cast<PFN_##func>(vkGetDeviceProcAddr(device, #func))

Extension::Extension(const char* extensionName) : extensionName(extensionName), support(false)
{
}

void Extension::check(std::vector<VkExtensionProperties>& supportedExtensions)
{
    for (auto& supportedExtension : supportedExtensions)
    {
        if (strcmp(supportedExtension.extensionName, extensionName) == 0)
        {
            support = true;
            break;
        }
    }
}

void Extension::add(std::vector<const char*>& requestedExtensions)
{
    if (support)
    {
        LOGD("Extension %s is supported", extensionName);
        requestedExtensions.push_back(extensionName);
    }
}

InstanceExtension::InstanceExtension(const char* extensionName) : Extension(extensionName)
{
}

DeviceExtension::DeviceExtension(const char* extensionName) : Extension(extensionName)
{
}

// VK_KHR_portability_enumeration
PortabilityEnumeration::PortabilityEnumeration() : InstanceExtension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)
{
}

// VK_KHR_get_physical_device_properties2
PFN_vkGetPhysicalDeviceProperties2KHR vkGetPhysicalDeviceProperties2KHR;
PhysicalDeviceProperties2Extension::PhysicalDeviceProperties2Extension()
    : InstanceExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)
{
}

void PhysicalDeviceProperties2Extension::fetch(VkInstance instance)
{
    if (support)
    {
        GET_INSTANCE_PROC(instance, vkGetPhysicalDeviceProperties2KHR);
    }
}

// VK_EXT_debug_utils
PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
DebugUtils::DebugUtils() : InstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
{
}

void DebugUtils::fetch(VkInstance instance)
{
    if (support)
    {
        GET_INSTANCE_PROC(instance, vkCreateDebugUtilsMessengerEXT);
        GET_INSTANCE_PROC(instance, vkDestroyDebugUtilsMessengerEXT);
    }
}

DebugReportExtension::DebugReportExtension() : InstanceExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME)
{
}

InstanceExtension* ExtensionFactory::createInstanceExtension(ExtensionName extensionName)
{
    switch (extensionName)
    {
    case ExtensionName::PhysicalDeviceProperties2:
        return new PhysicalDeviceProperties2Extension();
    case ExtensionName::PortabilityEnumeration:
        return new PortabilityEnumeration();
    case ExtensionName::DebugUtils:
        return new DebugUtils();
    case ExtensionName::DebugReport:
        return new DebugReportExtension();
    default:
        UNREACHABLE();
        return nullptr;
    }
}

DeviceExtension* ExtensionFactory::createDeviceExtension(ExtensionName extensionName)
{
    switch (extensionName)
    {
    case ExtensionName::Swapchain:
        return new SwapchainExtension();
    case ExtensionName::DebugMarker:
        return new DebugMarkerExtension();
    case ExtensionName::AccelerationStructure:
        return new AccelerationStructureExtension();
    case ExtensionName::RayTracingPipeline:
        return new RayTracingPipelineExtension();
    case ExtensionName::DeviceAddress:
        return new DeviceAddressExtension();
    case ExtensionName::DeferredHostOperations:
        return new DeferredHostOperationsExtension();
    case ExtensionName::RayQuery:
        return new RayQueryExtension();
    case ExtensionName::DescriptorIndexing:
        return new DescriptorIndexingExtension();
    case ExtensionName::Spirv_1_4:
        return new Spirv_1_4_Extension();
    case ExtensionName::PortabilitySubset:
        return new PortabilitySubsetExtension();
    default:
        UNREACHABLE();
        return nullptr;
    }
}

// VK_KHR_swapchain
SwapchainExtension::SwapchainExtension() : DeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
{
}

// VK_EXT_debug_marker
DebugMarkerExtension::DebugMarkerExtension() : DeviceExtension(VK_EXT_DEBUG_MARKER_EXTENSION_NAME)
{
}

PFN_vkDebugMarkerSetObjectTagEXT vkDebugMarkerSetObjectTagEXT;
PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectNameEXT;
PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBeginEXT;
PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEndEXT;
PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsertEXT;
void DebugMarkerExtension::fetch(VkDevice device)
{
    if (support)
    {
        GET_DEVICE_PROC(device, vkDebugMarkerSetObjectTagEXT);
        GET_DEVICE_PROC(device, vkDebugMarkerSetObjectNameEXT);
        GET_DEVICE_PROC(device, vkCmdDebugMarkerBeginEXT);
        GET_DEVICE_PROC(device, vkCmdDebugMarkerEndEXT);
        GET_DEVICE_PROC(device, vkCmdDebugMarkerInsertEXT);
    }
}

// VK_KHR_acceleration_structure
PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR;
PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;

AccelerationStructureExtension::AccelerationStructureExtension()
    : DeviceExtension(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME), accelerationStructureFeatures({}),
      physicalDeviceAccelerationStructureProperties({})
{
}

void AccelerationStructureExtension::feature(void**& chain)
{
    if (support)
    {
        accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
        accelerationStructureFeatures.accelerationStructure = true;
        *chain = &accelerationStructureFeatures;
        chain = &accelerationStructureFeatures.pNext;
    }
}

void AccelerationStructureExtension::fetch(VkDevice device)
{
    if (support)
    {
        GET_DEVICE_PROC(device, vkCreateAccelerationStructureKHR);
        GET_DEVICE_PROC(device, vkDestroyAccelerationStructureKHR);
        GET_DEVICE_PROC(device, vkGetAccelerationStructureBuildSizesKHR);
        GET_DEVICE_PROC(device, vkGetAccelerationStructureDeviceAddressKHR);
        GET_DEVICE_PROC(device, vkBuildAccelerationStructuresKHR);
        GET_DEVICE_PROC(device, vkCmdBuildAccelerationStructuresKHR);
        GET_DEVICE_PROC(device, vkGetRayTracingShaderGroupHandlesKHR);
    }
}

void AccelerationStructureExtension::property(void**& chain)
{
    if (support)
    {
        physicalDeviceAccelerationStructureProperties = {};
        physicalDeviceAccelerationStructureProperties.sType =
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;

        *chain = &physicalDeviceAccelerationStructureProperties;
        chain = &physicalDeviceAccelerationStructureProperties.pNext;
    }
}

void* AccelerationStructureExtension::getProperty()
{
    return &physicalDeviceAccelerationStructureProperties;
}

// VK_KHR_ray_tracing_pipeline
PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;

RayTracingPipelineExtension::RayTracingPipelineExtension()
    : DeviceExtension(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME), rayTracingFeatures(),
      physicalDeviceRayTracingPipelineProperties()
{
}

void RayTracingPipelineExtension::feature(void**& chain)
{
    if (support)
    {
        rayTracingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        rayTracingFeatures.rayTracingPipeline = true;
        *chain = &rayTracingFeatures;
        chain = &rayTracingFeatures.pNext;
    }
}

void RayTracingPipelineExtension::fetch(VkDevice device)
{
    if (support)
    {
        GET_DEVICE_PROC(device, vkCmdTraceRaysKHR);
        GET_DEVICE_PROC(device, vkCreateRayTracingPipelinesKHR);
    }
}

void RayTracingPipelineExtension::property(void**& chain)
{
    if (support)
    {
        physicalDeviceRayTracingPipelineProperties.sType =
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

        *chain = &physicalDeviceRayTracingPipelineProperties;
        chain = &physicalDeviceRayTracingPipelineProperties.pNext;
    }
}

void* RayTracingPipelineExtension::getProperty()
{
    return &physicalDeviceRayTracingPipelineProperties;
}

// VK_KHR_buffer_device_address
PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;

DeviceAddressExtension::DeviceAddressExtension()
    : DeviceExtension(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME), bufferDeviceAddressFeatures()
{
}

void DeviceAddressExtension::feature(void**& chain)
{
    if (support)
    {
        bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
        bufferDeviceAddressFeatures.bufferDeviceAddress = true;
        *chain = &bufferDeviceAddressFeatures;
        chain = &bufferDeviceAddressFeatures.pNext;
    }
}

void DeviceAddressExtension::fetch(VkDevice device)
{
    if (support)
    {
        GET_DEVICE_PROC(device, vkGetBufferDeviceAddressKHR);
    }
}

PFN_vkCreateDeferredOperationKHR vkCreateDeferredOperationKHR;
PFN_vkDestroyDeferredOperationKHR vkDestroyDeferredOperationKHR;
PFN_vkGetDeferredOperationMaxConcurrencyKHR vkGetDeferredOperationMaxConcurrencyKHR;
PFN_vkGetDeferredOperationResultKHR vkGetDeferredOperationResultKHR;
PFN_vkDeferredOperationJoinKHR vkDeferredOperationJoinKHR;

DeferredHostOperationsExtension::DeferredHostOperationsExtension()
    : DeviceExtension(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME)
{
}

void DeferredHostOperationsExtension::fetch(VkDevice device)
{
    if (support)
    {
        GET_DEVICE_PROC(device, vkCreateDeferredOperationKHR);
        GET_DEVICE_PROC(device, vkDestroyDeferredOperationKHR);
        GET_DEVICE_PROC(device, vkGetDeferredOperationMaxConcurrencyKHR);
        GET_DEVICE_PROC(device, vkGetDeferredOperationResultKHR);
        GET_DEVICE_PROC(device, vkDeferredOperationJoinKHR);
    }
}

// VK_KHR_ray_query
RayQueryExtension::RayQueryExtension() : DeviceExtension(VK_KHR_RAY_QUERY_EXTENSION_NAME), rayQueryFeatures()
{
}

void RayQueryExtension::feature(void**& chain)
{
    if (support)
    {
        rayQueryFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
        rayQueryFeatures.rayQuery = true;
        *chain = &rayQueryFeatures;
        chain = &rayQueryFeatures.pNext;
    }
}

DescriptorIndexingExtension::DescriptorIndexingExtension()
    : DeviceExtension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME), descriptorIndexingFeatures(),
      descriptorIndexingProperties()
{
}

void DescriptorIndexingExtension::feature(void**& chain)
{
    if (support)
    {
        descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        /*
        descriptorIndexingFeatures.shaderInputAttachmentArrayDynamicIndexing = true;
        descriptorIndexingFeatures.shaderUniformTexelBufferArrayDynamicIndexing = true;
        descriptorIndexingFeatures.shaderStorageTexelBufferArrayDynamicIndexing = true;
        descriptorIndexingFeatures.shaderUniformBufferArrayNonUniformIndexing = true;
        descriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = true;
        descriptorIndexingFeatures.shaderStorageBufferArrayNonUniformIndexing = true;
        descriptorIndexingFeatures.shaderStorageImageArrayNonUniformIndexing = true;
        descriptorIndexingFeatures.shaderInputAttachmentArrayNonUniformIndexing = true;
        descriptorIndexingFeatures.shaderUniformTexelBufferArrayNonUniformIndexing = true;
        descriptorIndexingFeatures.shaderStorageTexelBufferArrayNonUniformIndexing = true;
        descriptorIndexingFeatures.descriptorBindingUniformBufferUpdateAfterBind = true;
        descriptorIndexingFeatures.descriptorBindingSampledImageUpdateAfterBind = true;
        descriptorIndexingFeatures.descriptorBindingStorageImageUpdateAfterBind = true;
        descriptorIndexingFeatures.descriptorBindingStorageBufferUpdateAfterBind = true;
        descriptorIndexingFeatures.descriptorBindingUniformTexelBufferUpdateAfterBind = true;
        descriptorIndexingFeatures.descriptorBindingStorageTexelBufferUpdateAfterBind = true;
        descriptorIndexingFeatures.descriptorBindingUpdateUnusedWhilePending = true;
        descriptorIndexingFeatures.descriptorBindingPartiallyBound = true;
        descriptorIndexingFeatures.descriptorBindingVariableDescriptorCount = true;
        descriptorIndexingFeatures.runtimeDescriptorArray = true;
        */
        *chain = &descriptorIndexingFeatures;
        chain = &descriptorIndexingFeatures.pNext;
    }
}

void DescriptorIndexingExtension::property(void**& chain)
{
    if (support)
    {
        descriptorIndexingProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT;

        *chain = &descriptorIndexingProperties;
        chain = &descriptorIndexingProperties.pNext;
    }
}

void* DescriptorIndexingExtension::getProperty()
{
    return &descriptorIndexingProperties;
}

Spirv_1_4_Extension::Spirv_1_4_Extension() : DeviceExtension(VK_KHR_SPIRV_1_4_EXTENSION_NAME)
{
}

/*
#define VK_KHR_portability_subset 1
#define VK_KHR_PORTABILITY_SUBSET_SPEC_VERSION 1
#define VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME "VK_KHR_portability_subset"
typedef struct VkPhysicalDevicePortabilitySubsetFeaturesKHR {
    VkStructureType    sType;
    void*              pNext;
    VkBool32           constantAlphaColorBlendFactors;
    VkBool32           events;
    VkBool32           imageViewFormatReinterpretation;
    VkBool32           imageViewFormatSwizzle;
    VkBool32           imageView2DOn3DImage;
    VkBool32           multisampleArrayImage;
    VkBool32           mutableComparisonSamplers;
    VkBool32           pointPolygons;
    VkBool32           samplerMipLodBias;
    VkBool32           separateStencilMaskRef;
    VkBool32           shaderSampleRateInterpolationFunctions;
    VkBool32           tessellationIsolines;
    VkBool32           tessellationPointMode;
    VkBool32           triangleFans;
    VkBool32           vertexAttributeAccessBeyondStride;
} VkPhysicalDevicePortabilitySubsetFeaturesKHR;

typedef struct VkPhysicalDevicePortabilitySubsetPropertiesKHR {
    VkStructureType    sType;
    void*              pNext;
    uint32_t           minVertexInputBindingStrideAlignment;
} VkPhysicalDevicePortabilitySubsetPropertiesKHR;
*/
// VK_KHR_portability_subset
PortabilitySubsetExtension::PortabilitySubsetExtension() : DeviceExtension(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME)
{
}

} // namespace vk