#pragma once

#define VK_ENABLE_BETA_EXTENSIONS
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_core.h"
#include <map>
#include <vector>

namespace vk
{
enum class ExtensionName
{
    Swapchain,
    DebugMarker,
    DebugUtils,
    DebugReport,
    AccelerationStructure,
    RayTracingPipeline,
    DeviceAddress,
    DeferredHostOperations,
    PhysicalDeviceProperties2,
    PortabilityEnumeration,
    PortabilitySubset,
    RayQuery,
    DescriptorIndexing,
    Spirv_1_4
};

class Extension
{
  public:
    Extension(const char* extensionName);

    ~Extension() = default;

    void check(std::vector<VkExtensionProperties>& supportedExtensions);

    void add(std::vector<const char*>& requestedExtensions);

    inline bool isSupport()
    {
        return support;
    }

    virtual void* getProperty()
    {
        return nullptr;
    }

  private:
    const char* extensionName;

  protected:
    bool support;
};

class InstanceExtension : public Extension
{
  public:
    InstanceExtension(const char* extensionName);

    virtual void fetch(VkInstance instance)
    {
    }
};

class DeviceExtension : public Extension
{
  public:
    DeviceExtension(const char* extensionName);

    virtual void feature(void**& chain)
    {
    }

    virtual void fetch(VkDevice device)
    {
    }

    virtual void property(void**& chain)
    {
    }

    virtual void* getProperty()
    {
        return nullptr;
    }
};

class ExtensionFactory
{
  public:
    static InstanceExtension* createInstanceExtension(ExtensionName deviceExtensionName);
    static DeviceExtension* createDeviceExtension(ExtensionName deviceExtensionName);
};

/*
List of Instance extensions
*/

// VK_KHR_portability_enumeration
class PortabilityEnumeration : public InstanceExtension
{
  public:
    PortabilityEnumeration();
};

// VK_KHR_get_physical_device_properties2
extern PFN_vkGetPhysicalDeviceProperties2KHR vkGetPhysicalDeviceProperties2KHR;
class PhysicalDeviceProperties2Extension : public InstanceExtension
{
  public:
    PhysicalDeviceProperties2Extension();

    void fetch(VkInstance instance) override final;
};

// VK_EXT_debug_utils
extern PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
extern PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
class DebugUtils : public InstanceExtension
{
  public:
    DebugUtils();

    void fetch(VkInstance instance) override final;
};

// VK_EXT_DEBUG_REPORT
class DebugReportExtension : public InstanceExtension
{
  public:
    DebugReportExtension();
};

/*
List of Device extensions
*/

// VK_KHR_swapchain
class SwapchainExtension : public DeviceExtension
{
  public:
    SwapchainExtension();
};

// VK_EXT_debug_marker
extern PFN_vkDebugMarkerSetObjectTagEXT vkDebugMarkerSetObjectTagEXT;
extern PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectNameEXT;
extern PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBeginEXT;
extern PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEndEXT;
extern PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsertEXT;

class DebugMarkerExtension : public DeviceExtension
{
  public:
    DebugMarkerExtension();

    void fetch(VkDevice device) override;
};

// VK_KHR_acceleration_structure
extern PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
extern PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
extern PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
extern PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
extern PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR;
extern PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
extern PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;

class AccelerationStructureExtension : public DeviceExtension
{
  public:
    AccelerationStructureExtension();

    void feature(void**& chain) override;

    void fetch(VkDevice device) override;

    void property(void**& chain) override;

    virtual void* getProperty() override;

  private:
    VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures;
    VkPhysicalDeviceAccelerationStructurePropertiesKHR physicalDeviceAccelerationStructureProperties;
};

// VK_KHR_ray_tracing_pipeline
extern PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
extern PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;

class RayTracingPipelineExtension : public DeviceExtension
{
  public:
    RayTracingPipelineExtension();

    void feature(void**& chain) override;

    void fetch(VkDevice device) override;

    void property(void**& chain) override;

    virtual void* getProperty() override;

  private:
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingFeatures;
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR physicalDeviceRayTracingPipelineProperties;
};

// VK_KHR_buffer_device_address
extern PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;

class DeviceAddressExtension : public DeviceExtension
{
  public:
    DeviceAddressExtension();

    void feature(void**& chain) override;

    void fetch(VkDevice device) override;

  private:
    VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures;
};

// VK_KHR_deferred_host_operations
extern PFN_vkCreateDeferredOperationKHR vkCreateDeferredOperationKHR;
extern PFN_vkDestroyDeferredOperationKHR vkDestroyDeferredOperationKHR;
extern PFN_vkGetDeferredOperationMaxConcurrencyKHR vkGetDeferredOperationMaxConcurrencyKHR;
extern PFN_vkGetDeferredOperationResultKHR vkGetDeferredOperationResultKHR;
extern PFN_vkDeferredOperationJoinKHR vkDeferredOperationJoinKHR;

class DeferredHostOperationsExtension : public DeviceExtension
{
  public:
    DeferredHostOperationsExtension();

    void fetch(VkDevice device) override;
};

// VK_KHR_ray_query
class RayQueryExtension : public DeviceExtension
{
  public:
    RayQueryExtension();

    void feature(void**& chain) override;

  private:
    VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures;
};

class DescriptorIndexingExtension : public DeviceExtension
{
  public:
    DescriptorIndexingExtension();

    void feature(void**& chain) override;

    void property(void**& chain) override;

    virtual void* getProperty() override;

  private:
    VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures;
    VkPhysicalDeviceDescriptorIndexingProperties descriptorIndexingProperties;
};

// VK_KHR_spirv_1_4
class Spirv_1_4_Extension : public DeviceExtension
{
  public:
    Spirv_1_4_Extension();
};

// VK_KHR_portability_subset
class PortabilitySubsetExtension : public DeviceExtension
{
  public:
    PortabilitySubsetExtension();
};

} // namespace vk