// Which of DXVK 3.2's required Vulkan features MoltenVK has (round 28, docs/fork/P2CE_RENDERER_DECISION.md).
// make p2ce-moltenvk-features: needs MoltenVK and DXVK's Vulkan headers under deps/ (not part of make check).
#include <vulkan/vulkan.h>
#include <stdio.h>
#include <string.h>
#define F(s, f) printf("%-48s %s\n", #f, (s).f ? "yes" : "NO")
int main(void) {
    const char *ext[] = {"VK_KHR_get_physical_device_properties2"};
    VkApplicationInfo app = {VK_STRUCTURE_TYPE_APPLICATION_INFO, 0, "q", 1, "q", 1, VK_API_VERSION_1_3};
    VkInstanceCreateInfo ci = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, 0, 0, &app, 0, 0, 1, ext};
    VkInstance inst; if (vkCreateInstance(&ci, 0, &inst)) { puts("no instance"); return 1; }
    uint32_t n = 1; VkPhysicalDevice pd; vkEnumeratePhysicalDevices(inst, &n, &pd);
    VkPhysicalDeviceProperties p; vkGetPhysicalDeviceProperties(pd, &p);
    printf("%s, Vulkan %u.%u.%u, maxPushConstants %u\n", p.deviceName, VK_API_VERSION_MAJOR(p.apiVersion), VK_API_VERSION_MINOR(p.apiVersion), VK_API_VERSION_PATCH(p.apiVersion), p.limits.maxPushConstantsSize);
    VkPhysicalDeviceRobustness2FeaturesEXT rb = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT};
    VkPhysicalDeviceDepthClipEnableFeaturesEXT dc = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT, &rb};
    VkPhysicalDeviceTransformFeedbackFeaturesEXT tf = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT, &dc};
    VkPhysicalDeviceMaintenance5FeaturesKHR m5 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES_KHR, &tf};
    VkPhysicalDeviceMaintenance6FeaturesKHR m6 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_FEATURES_KHR, &m5};
    VkPhysicalDeviceVulkan13Features v13 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, &m6};
    VkPhysicalDeviceVulkan12Features v12 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &v13};
    VkPhysicalDeviceVulkan11Features v11 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &v12};
    VkPhysicalDeviceFeatures2 f2 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &v11};
    vkGetPhysicalDeviceFeatures2(pd, &f2);
    VkPhysicalDeviceFeatures c = f2.features;
    F(c, depthBiasClamp); F(c, depthClamp); F(c, dualSrcBlend); F(c, fillModeNonSolid); F(c, fragmentStoresAndAtomics);
    F(c, fullDrawIndexUint32); F(c, geometryShader); F(c, imageCubeArray); F(c, independentBlend); F(c, multiDrawIndirect);
    F(c, multiViewport); F(c, occlusionQueryPrecise); F(c, robustBufferAccess); F(c, sampleRateShading); F(c, samplerAnisotropy);
    F(c, shaderClipDistance); F(c, shaderCullDistance); F(c, shaderImageGatherExtended); F(c, shaderInt16); F(c, shaderInt64);
    F(c, shaderSampledImageArrayDynamicIndexing); F(c, textureCompressionBC); F(c, tessellationShader);
    F(v11, shaderDrawParameters); F(v11, storageBuffer16BitAccess);
    F(v12, bufferDeviceAddress); F(v12, descriptorIndexing); F(v12, storageBuffer8BitAccess);
    F(v12, descriptorBindingSampledImageUpdateAfterBind); F(v12, descriptorBindingUpdateUnusedWhilePending);
    F(v12, descriptorBindingPartiallyBound); F(v12, hostQueryReset); F(v12, runtimeDescriptorArray);
    F(v12, samplerMirrorClampToEdge); F(v12, scalarBlockLayout); F(v12, shaderInt8); F(v12, timelineSemaphore);
    F(v12, uniformBufferStandardLayout); F(v12, vulkanMemoryModel);
    F(v13, inlineUniformBlock); F(v13, computeFullSubgroups); F(v13, dynamicRendering); F(v13, maintenance4);
    F(v13, shaderDemoteToHelperInvocation); F(v13, shaderZeroInitializeWorkgroupMemory); F(v13, subgroupSizeControl);
    F(v13, synchronization2);
    F(dc, depthClipEnable); F(rb, robustBufferAccess2); F(rb, nullDescriptor); F(m5, maintenance5); F(m6, maintenance6);
    F(tf, transformFeedback);
    uint32_t en = 0; vkEnumerateDeviceExtensionProperties(pd, 0, &en, 0);
    VkExtensionProperties e[512]; if (en > 512) en = 512; vkEnumerateDeviceExtensionProperties(pd, 0, &en, e);
    const char *want[] = {"VK_KHR_load_store_op_none", "VK_KHR_swapchain", "VK_EXT_robustness2", "VK_KHR_maintenance5", "VK_KHR_maintenance6", "VK_EXT_depth_clip_enable", "VK_EXT_transform_feedback"};
    for (unsigned w = 0; w < sizeof want / sizeof *want; ++w) { int h = 0; for (uint32_t i = 0; i < en; ++i) h |= !strcmp(e[i].extensionName, want[w]); printf("%-48s %s\n", want[w], h ? "yes" : "NO"); }
    return 0;
}
