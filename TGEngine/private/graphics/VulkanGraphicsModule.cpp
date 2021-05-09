#include "../../public/graphics/VulkanGraphicsModule.hpp"

#include "../../public/Error.hpp"
#include <array>
#include <glslang/Include/intermediate.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>

#include <iostream>

#ifdef WIN32
#include <Windows.h>
#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 1
#define VK_USE_PLATFORM_WIN32_KHR 1
#endif // WIN32
#include "../../public/graphics/GameGraphicsModule.hpp"

#include "../../public/Util.hpp"
#include <vector>
#define VULKAN_HPP_HAS_SPACESHIP_OPERATOR
#include <unordered_set>
#include <vulkan/vulkan.hpp>

namespace tge::graphics {

constexpr TBuiltInResource DefaultTBuiltInResource = {
    /* .MaxLights = */ 32,
    /* .MaxClipPlanes = */ 6,
    /* .MaxTextureUnits = */ 32,
    /* .MaxTextureCoords = */ 32,
    /* .MaxVertexAttribs = */ 64,
    /* .MaxVertexUniformComponents = */ 4096,
    /* .MaxVaryingFloats = */ 64,
    /* .MaxVertexTextureImageUnits = */ 32,
    /* .MaxCombinedTextureImageUnits = */ 80,
    /* .MaxTextureImageUnits = */ 32,
    /* .MaxFragmentUniformComponents = */ 4096,
    /* .MaxDrawBuffers = */ 32,
    /* .MaxVertexUniformVectors = */ 128,
    /* .MaxVaryingVectors = */ 8,
    /* .MaxFragmentUniformVectors = */ 16,
    /* .MaxVertexOutputVectors = */ 16,
    /* .MaxFragmentInputVectors = */ 15,
    /* .MinProgramTexelOffset = */ -8,
    /* .MaxProgramTexelOffset = */ 7,
    /* .MaxClipDistances = */ 8,
    /* .MaxComputeWorkGroupCountX = */ 65535,
    /* .MaxComputeWorkGroupCountY = */ 65535,
    /* .MaxComputeWorkGroupCountZ = */ 65535,
    /* .MaxComputeWorkGroupSizeX = */ 1024,
    /* .MaxComputeWorkGroupSizeY = */ 1024,
    /* .MaxComputeWorkGroupSizeZ = */ 64,
    /* .MaxComputeUniformComponents = */ 1024,
    /* .MaxComputeTextureImageUnits = */ 16,
    /* .MaxComputeImageUniforms = */ 8,
    /* .MaxComputeAtomicCounters = */ 8,
    /* .MaxComputeAtomicCounterBuffers = */ 1,
    /* .MaxVaryingComponents = */ 60,
    /* .MaxVertexOutputComponents = */ 64,
    /* .MaxGeometryInputComponents = */ 64,
    /* .MaxGeometryOutputComponents = */ 128,
    /* .MaxFragmentInputComponents = */ 128,
    /* .MaxImageUnits = */ 8,
    /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
    /* .MaxCombinedShaderOutputResources = */ 8,
    /* .MaxImageSamples = */ 0,
    /* .MaxVertexImageUniforms = */ 0,
    /* .MaxTessControlImageUniforms = */ 0,
    /* .MaxTessEvaluationImageUniforms = */ 0,
    /* .MaxGeometryImageUniforms = */ 0,
    /* .MaxFragmentImageUniforms = */ 8,
    /* .MaxCombinedImageUniforms = */ 8,
    /* .MaxGeometryTextureImageUnits = */ 16,
    /* .MaxGeometryOutputVertices = */ 256,
    /* .MaxGeometryTotalOutputComponents = */ 1024,
    /* .MaxGeometryUniformComponents = */ 1024,
    /* .MaxGeometryVaryingComponents = */ 64,
    /* .MaxTessControlInputComponents = */ 128,
    /* .MaxTessControlOutputComponents = */ 128,
    /* .MaxTessControlTextureImageUnits = */ 16,
    /* .MaxTessControlUniformComponents = */ 1024,
    /* .MaxTessControlTotalOutputComponents = */ 4096,
    /* .MaxTessEvaluationInputComponents = */ 128,
    /* .MaxTessEvaluationOutputComponents = */ 128,
    /* .MaxTessEvaluationTextureImageUnits = */ 16,
    /* .MaxTessEvaluationUniformComponents = */ 1024,
    /* .MaxTessPatchComponents = */ 120,
    /* .MaxPatchVertices = */ 32,
    /* .MaxTessGenLevel = */ 64,
    /* .MaxViewports = */ 16,
    /* .MaxVertexAtomicCounters = */ 0,
    /* .MaxTessControlAtomicCounters = */ 0,
    /* .MaxTessEvaluationAtomicCounters = */ 0,
    /* .MaxGeometryAtomicCounters = */ 0,
    /* .MaxFragmentAtomicCounters = */ 8,
    /* .MaxCombinedAtomicCounters = */ 8,
    /* .MaxAtomicCounterBindings = */ 1,
    /* .MaxVertexAtomicCounterBuffers = */ 0,
    /* .MaxTessControlAtomicCounterBuffers = */ 0,
    /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
    /* .MaxGeometryAtomicCounterBuffers = */ 0,
    /* .MaxFragmentAtomicCounterBuffers = */ 1,
    /* .MaxCombinedAtomicCounterBuffers = */ 1,
    /* .MaxAtomicCounterBufferSize = */ 16384,
    /* .MaxTransformFeedbackBuffers = */ 4,
    /* .MaxTransformFeedbackInterleavedComponents = */ 64,
    /* .MaxCullDistances = */ 8,
    /* .MaxCombinedClipAndCullDistances = */ 8,
    /* .MaxSamples = */ 4,
    /* .maxMeshOutputVerticesNV = */ 256,
    /* .maxMeshOutputPrimitivesNV = */ 512,
    /* .maxMeshWorkGroupSizeX_NV = */ 32,
    /* .maxMeshWorkGroupSizeY_NV = */ 1,
    /* .maxMeshWorkGroupSizeZ_NV = */ 1,
    /* .maxTaskWorkGroupSizeX_NV = */ 32,
    /* .maxTaskWorkGroupSizeY_NV = */ 1,
    /* .maxTaskWorkGroupSizeZ_NV = */ 1,
    /* .maxMeshViewCountNV = */ 4,
    /* .maxDualSourceDrawBuffersEXT = */ 1,

    /* .limits = */
    {
        /* .nonInductiveForLoops = */ 1,
        /* .whileLoops = */ 1,
        /* .doWhileLoops = */ 1,
        /* .generalUniformIndexing = */ 1,
        /* .generalAttributeMatrixVectorIndexing = */ 1,
        /* .generalVaryingIndexing = */ 1,
        /* .generalSamplerIndexing = */ 1,
        /* .generalVariableIndexing = */ 1,
        /* .generalConstantMatrixVectorIndexing = */ 1,
    }};

constexpr std::array layerToEnable = {"VK_LAYER_KHRONOS_validation",
                                      "VK_LAYER_VALVE_steam_overlay",
                                      "VK_LAYER_NV_optimus"};

constexpr std::array extensionToEnable = {"VK_KHR_SURFACE_EXTENSION_NAME",
                                          "VK_KHR_surface"
#ifdef WIN32
                                          ,
                                          "VK_KHR_win32_surface"
#endif // WIN32
#ifdef DEBUG
                                          ,
                                          "VK_EXT_debug_utils"
#endif

};

using namespace vk;

std::vector<GraphicsPipelineCreateInfo> pipelineCreateInfos;
Result verror = Result::eSuccess;

#define VERROR(rslt)                                                           \
  if (rslt != Result::eSuccess) {                                              \
    verror = rslt;                                                             \
    main::error = main::Error::VULKAN_ERROR;                                   \
    printf("Vulkan error %d!", (uint32_t)verror);                              \
  }

class VulkanGraphicsModule : public tge::main::Module, public APILayer {
public:
  VulkanGraphicsModule() : gamegraphics(GameGraphicsModule(this)){};

private:
  Instance instance;
  PhysicalDevice physicalDevice;
  Device device;
  SurfaceKHR surface;
  char *window = nullptr;
  SurfaceFormatKHR format;
  SwapchainKHR swapchain;
  std::vector<Image> images;
  RenderPass renderpass;
  std::vector<ImageView> imageviews;
  std::vector<Framebuffer> framebuffer;
  CommandPool pool;
  std::vector<CommandBuffer> cmdbuffer;
  std::vector<Pipeline> pipelines;
  Queue queue;
  Semaphore waitSemaphore;
  Semaphore signalSemaphore;
  GameGraphicsModule gamegraphics;
  std::vector<ShaderModule> shaderModules;
  uint32_t memoryTypeHostVisibleCoherent;
  uint32_t memoryTypeDeviceLocal;

#ifdef DEBUG
  DebugUtilsMessengerEXT debugMessenger;
#endif

  main::Error init();

  void tick(double time);

  void destroy();

  main::Error createWindowAndGetSurface();

  main::Error pushMaterials(const size_t materialcount,
                            const Material *materials);

  main::Error pushVertexData(const size_t dataCount, const uint8_t **data,
                             const size_t *dataSizes);

  GameGraphicsModule *getGraphicsModule() { return &gamegraphics; }
};

inline EShLanguage getLang(const std::string &str) {
  if (str.compare("vert") == 0)
    return EShLanguage::EShLangVertex;
  if (str.compare("frag") == 0)
    return EShLanguage::EShLangFragment;
  throw std::runtime_error(std::string("Couldn't find EShLanguage for ") + str);
}

inline ShaderStageFlagBits getStageFromLang(const EShLanguage lang) {
  switch (lang) {
  case EShLanguage::EShLangVertex:
    return ShaderStageFlagBits::eVertex;
  case EShLanguage::EShLangFragment:
    return ShaderStageFlagBits::eFragment;
  }
  throw std::runtime_error(
      std::string("Couldn't find ShaderStageFlagBits for EShLanguage ") +
      std::to_string(lang));
}

inline Format getFormatFromElf(const glslang::TType &format) {
  if (format.isVector() &&
      format.getBasicType() == glslang::TBasicType::EbtFloat) {
    if (format.getVectorSize() == 2)
      return Format::eR32G32Sfloat;
    if (format.getVectorSize() == 4)
      return Format::eR32G32B32A32Sfloat;
  }
  throw std::runtime_error(std::string("Couldn't find Format for TType ") +
                           format.getTypeName().c_str());
}

inline uint32_t getSizeFromFormat(const Format format) {
  switch (format) {
  case Format::eR32G32Sfloat:
    return 8;
  case Format::eR32G32B32A32Sfloat:
    return 16;
  }
  throw std::runtime_error(std::string("Couldn't find size for Format ") +
                           std::to_string((size_t)format));
}

struct VulkanShaderPipe {
  std::vector<std::pair<std::vector<uint32_t>, ShaderStageFlagBits>> shader;
  std::vector<VertexInputBindingDescription> vertexInputBindings;
  std::vector<VertexInputAttributeDescription> vertexInputAttributes;
  PipelineVertexInputStateCreateInfo inputStateCreateInfo;
  PipelineRasterizationStateCreateInfo rasterization;
  PipelineLayoutCreateInfo layoutCreateInfo;
};

struct ShaderAnalizer : public glslang::TIntermTraverser {

  VulkanShaderPipe *shaderPipe;

  ShaderAnalizer(VulkanShaderPipe *pipe)
      : glslang::TIntermTraverser(false, true, false), shaderPipe(pipe) {
    ioVars.reserve(10);
  }
  std::unordered_set<size_t> ioVars;

#define NO_BINDING_GIVEN 65535
#define NO_LAYOUT_GIVEN 4095

  void visitSymbol(glslang::TIntermSymbol *symbol) {
    const auto &qualifier = symbol->getQualifier();
    if (ioVars.contains(symbol->getId()))
      return;
    ioVars.emplace(symbol->getId());
    if (qualifier.layoutLocation < NO_LAYOUT_GIVEN) {
      if (qualifier.storage == glslang::TStorageQualifier::EvqVaryingIn) {
        const auto bind = qualifier.layoutBinding == NO_BINDING_GIVEN
                              ? 0
                              : qualifier.layoutBinding;
        shaderPipe->vertexInputAttributes.push_back(
            VertexInputAttributeDescription(
                qualifier.layoutLocation, bind,
                getFormatFromElf(symbol->getType())));
      }
    }
  }

  void post() {
    const auto beginItr = shaderPipe->vertexInputAttributes.begin();
    const auto endItr = shaderPipe->vertexInputAttributes.end();

    for (auto &vert : shaderPipe->vertexInputAttributes) {
      for (auto itr = beginItr; itr != endItr; itr++) {
        if (itr->location < vert.location && itr->binding == vert.binding)
          vert.offset += getSizeFromFormat(itr->format);
      }

      auto beginItrBinding = shaderPipe->vertexInputBindings.begin();
      auto endItrBinding = shaderPipe->vertexInputBindings.end();
      auto fitr = std::find_if(
          beginItrBinding, endItrBinding,
          [bind = vert.binding](auto c) { return c.binding == bind; });
      if (fitr == endItrBinding) {
        const auto index = shaderPipe->vertexInputBindings.size();
        shaderPipe->vertexInputBindings.push_back(
            VertexInputBindingDescription(vert.binding, 0));
        fitr = shaderPipe->vertexInputBindings.begin() + index;
      }
      fitr->stride += getSizeFromFormat(vert.format);
    }

    shaderPipe->inputStateCreateInfo = PipelineVertexInputStateCreateInfo(
        {}, shaderPipe->vertexInputBindings.size(),
        shaderPipe->vertexInputBindings.data(),
        shaderPipe->vertexInputAttributes.size(),
        shaderPipe->vertexInputAttributes.data());
  }
};

VulkanShaderPipe *__implLoadShaderPipeAndCompile(
    std::vector<std::pair<std::unique_ptr<uint8_t[]>, EShLanguage>> &vector) {
  VulkanShaderPipe *shaderPipe = new VulkanShaderPipe();
  glslang::InitializeProcess();
  util::OnExit e1(glslang::FinalizeProcess);
  shaderPipe->shader.reserve(vector.size());

  for (auto &pair : vector) {
    const auto data = std::move(pair.first);
    const auto langName = pair.second;
    if (data.get() == nullptr) {
      delete shaderPipe;
      return nullptr;
    }
    glslang::TShader shader(langName);
    const auto ptr = data.get();
    shader.setStrings((const char *const *)(&ptr), 1);
    shader.setEnvInput(glslang::EShSourceGlsl, langName,
                       glslang::EShClientVulkan, 100);
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_0);
    shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);
    if (!shader.parse(&DefaultTBuiltInResource, 450, true,
                      EShMessages::EShMsgVulkanRules))
      return nullptr;
    printf("%s", shader.getInfoLog());

    const auto interm = shader.getIntermediate();
    const auto node = interm->getTreeRoot();
    if (langName == EShLangVertex) { // currently only vertex analyzation
      ShaderAnalizer analizer(shaderPipe);
      node->traverse(&analizer);
      analizer.post();
    }

    const auto index = shaderPipe->shader.size();
    shaderPipe->shader.push_back(
        std::pair(std::vector<uint32_t>(), getStageFromLang(langName)));
    glslang::GlslangToSpv(*interm, shaderPipe->shader[index].first);
  }

  return shaderPipe;
}

uint8_t *loadShaderPipeAndCompile(std::vector<std::string> &shadernames) {
  std::vector<std::pair<std::unique_ptr<uint8_t[]>, EShLanguage>> vector;
  vector.reserve(shadernames.size());
  for (const auto &name : shadernames) {
    const std::string abrivation = name.substr(name.size() - 4);
    auto path = fs::path(name);
    vector.push_back(
        std::pair(std::move(util::wholeFile(path)), getLang(abrivation)));
  }
  const auto loadedPipes = __implLoadShaderPipeAndCompile(vector);
  return (uint8_t *)loadedPipes;
}

constexpr PipelineInputAssemblyStateCreateInfo
    inputAssemblyCreateInfo({}, PrimitiveTopology::eTriangleList,
                            false); // For now constexpr

main::Error VulkanGraphicsModule::pushMaterials(const size_t materialcount,
                                                const Material *materials) {
  for (size_t i = 0; i < materialcount; i++) {
    const auto &material = materials[i];

    const auto shaderPipe = (VulkanShaderPipe *)material.costumShaderData;

    const auto firstIndex = shaderModules.size();

    std::vector<PipelineShaderStageCreateInfo> pipelineShaderStage;
    pipelineShaderStage.reserve(shaderPipe->shader.size());
    for (const auto &shaderPair : shaderPipe->shader) {
      const auto &shaderData = shaderPair.first;

      const ShaderModuleCreateInfo shaderModuleCreateInfo(
          {}, shaderData.size() * sizeof(uint32_t), shaderData.data());
      const auto shaderModule =
          device.createShaderModule(shaderModuleCreateInfo);
      shaderModules.push_back(shaderModule);
      pipelineShaderStage.push_back(PipelineShaderStageCreateInfo(
          {}, shaderPair.second, shaderModule, "main"));
    }

    shaderPipe->rasterization.lineWidth = 1;
    shaderPipe->rasterization.depthBiasEnable = false;
    shaderPipe->rasterization.rasterizerDiscardEnable = true;
    shaderPipe->rasterization.cullMode = CullModeFlagBits::eFront;

    const auto layout =
        device.createPipelineLayout(shaderPipe->layoutCreateInfo);

    const GraphicsPipelineCreateInfo gpipeCreateInfo(
        {}, pipelineShaderStage, &shaderPipe->inputStateCreateInfo,
        &inputAssemblyCreateInfo, {}, {}, &shaderPipe->rasterization, {}, {},
        {}, {}, layout, renderpass, 0);
    pipelineCreateInfos.push_back(gpipeCreateInfo);
  }
  return main::Error::NONE;
}

main::Error VulkanGraphicsModule::pushVertexData(const size_t dataCount,
                                                 const uint8_t **data,
                                                 const size_t *dataSizes) {
  std::vector<DeviceMemory> tempMemory;
  tempMemory.reserve(dataCount);
  std::vector<Buffer> tempBuffer;
  tempBuffer.reserve(dataCount);

  for (size_t i = 0; i < dataCount; i++) {
    const auto size = dataSizes[i];
    const auto dataptr = data[i];

    const BufferCreateInfo bufferCreateInfo(
        {}, size,
        BufferUsageFlagBits::eVertexBuffer | BufferUsageFlagBits::eTransferSrc,
        SharingMode::eExclusive);
    const auto intermBuffer = device.createBuffer(bufferCreateInfo);
    tempBuffer.push_back(intermBuffer);
    const auto memRequ = device.getBufferMemoryRequirements(intermBuffer);

    const MemoryAllocateInfo allocInfo(memRequ.size,
                                       memoryTypeHostVisibleCoherent);
    const auto hostVisibleMemory = device.allocateMemory(allocInfo);
    tempMemory.push_back(hostVisibleMemory);
    const auto mappedHandle = device.mapMemory(hostVisibleMemory, 0, VK_WHOLE_SIZE);
    memcpy(mappedHandle, dataptr, size);
    device.unmapMemory(hostVisibleMemory);

    const MemoryAllocateInfo allocLocalInfo(memRequ.size,
                                            memoryTypeDeviceLocal);
  }
  return main::Error::NONE;
}

#ifdef WIN32

LRESULT CALLBACK callback(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
  return DefWindowProc(hWnd, Msg, wParam, lParam);
}

main::Error VulkanGraphicsModule::createWindowAndGetSurface() {
  HMODULE systemHandle;
  if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_PIN, nullptr, &systemHandle))
    return main::Error::NO_MODULE_HANDLE;

  const auto windowProperties = gamegraphics.getWindowProperties();

  WNDCLASSEX wndclass;
  FillMemory(&wndclass, sizeof(WNDCLASSEX), 0);
  wndclass.cbSize = sizeof(WNDCLASSEX);
  wndclass.style = CS_ENABLE | CS_OWNDC | CS_HREDRAW;
  wndclass.lpfnWndProc = callback;
  wndclass.hInstance = systemHandle;
  wndclass.lpszClassName = ENGINE_NAME;

  auto regWndClass = RegisterClassEx(&wndclass);
  if (!regWndClass)
    return main::Error::COULD_NOT_CREATE_WINDOW_CLASS;

  auto window = CreateWindowEx(
      WS_EX_APPWINDOW, (LPCSTR)regWndClass, APPLICATION_NAME,
      WS_CLIPSIBLINGS | WS_CAPTION | WS_SIZEBOX | WS_SYSMENU,
      windowProperties.x, windowProperties.y, windowProperties.width,
      windowProperties.height, NULL, NULL, systemHandle, NULL);
  if (!window)
    return main::Error::COULD_NOT_CREATE_WINDOW;
  this->window = (char *)&window;
  ShowWindow(window, SW_SHOW);
  UpdateWindow(window);

  Win32SurfaceCreateInfoKHR surfaceCreateInfo({}, systemHandle, window);
  surface = instance.createWin32SurfaceKHR(surfaceCreateInfo);
  return main::Error::NONE;
}

#endif // WIN32

#ifdef DEBUG
VkBool32 debugMessage(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                      VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                      void *pUserData) {
  std::string severity =
      to_string((DebugUtilsMessageSeverityFlagBitsEXT)messageSeverity);
  std::string type = to_string((DebugUtilsMessageTypeFlagBitsEXT)messageTypes);

  printf("[%s][%s]: %s\n", severity.c_str(), type.c_str(),
         pCallbackData->pMessage);
  return VK_TRUE;
}
#endif

main::Error VulkanGraphicsModule::init() {

  const ApplicationInfo applicationInfo(APPLICATION_NAME, APPLICATION_VERSION,
                                        ENGINE_NAME, ENGINE_VERSION,
                                        VK_API_VERSION_1_0);

  const auto layerInfos = enumerateInstanceLayerProperties();
  std::vector<const char *> layerEnabled;
  for (const auto &layer : layerInfos) {
    const auto lname = layer.layerName.data();
    const auto enditr = layerToEnable.end();
    if (std::find_if(layerToEnable.begin(), enditr,
                     [&](auto in) { return strcmp(lname, in) == 0; }) != enditr)
      layerEnabled.push_back(lname);
  }

  const auto extensionInfos = enumerateInstanceExtensionProperties();
  std::vector<const char *> extensionEnabled;
  for (const auto &extension : extensionInfos) {
    const auto lname = extension.extensionName.data();
    const auto enditr = extensionToEnable.end();
    if (std::find_if(extensionToEnable.begin(), enditr,
                     [&](auto in) { return strcmp(lname, in) == 0; }) != enditr)
      extensionEnabled.push_back(lname);
  }

  const InstanceCreateInfo createInfo(
      {}, &applicationInfo, (uint32_t)layerEnabled.size(), layerEnabled.data(),
      (uint32_t)extensionEnabled.size(), extensionEnabled.data());
  this->instance = createInstance(createInfo);

#ifdef DEBUG
  DispatchLoaderDynamic stat;
  stat.vkCreateDebugUtilsMessengerEXT =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          instance, "vkCreateDebugUtilsMessengerEXT");
  const DebugUtilsMessengerCreateInfoEXT debugUtilsMsgCreateInfo(
      {},
      (DebugUtilsMessageSeverityFlagsEXT)
          FlagTraits<DebugUtilsMessageSeverityFlagBitsEXT>::allFlags,
      (DebugUtilsMessageTypeFlagsEXT)
          FlagTraits<DebugUtilsMessageTypeFlagBitsEXT>::allFlags,
      debugMessage);
  debugMessenger = instance.createDebugUtilsMessengerEXT(
      debugUtilsMsgCreateInfo, nullptr, stat);
#endif

  constexpr auto getScore = [](auto physDevice) {
    const auto properties = physDevice.getProperties();
    return properties.limits.maxImageDimension2D +
           (properties.deviceType == PhysicalDeviceType::eDiscreteGpu ? 1000
                                                                      : 0);
  };

  const auto physicalDevices = this->instance.enumeratePhysicalDevices();
  this->physicalDevice = *std::max_element(
      physicalDevices.begin(), physicalDevices.end(),
      [&](auto p1, auto p2) { return getScore(p1) < getScore(p2); });

  // just one queue for now
  const auto queueFamilys = this->physicalDevice.getQueueFamilyProperties();
  const auto bgnitr = queueFamilys.begin();
  const auto enditr = queueFamilys.end();
  const auto queueFamilyItr = std::find_if(bgnitr, enditr, [](auto queue) {
    return queue.queueFlags & QueueFlagBits::eGraphics;
  });
  if (queueFamilyItr == enditr)
    return main::Error::NO_GRAPHIC_QUEUE_FOUND;

  const auto queueFamilyIndex = (uint32_t)std::distance(bgnitr, queueFamilyItr);
  const auto &queueFamily = *queueFamilyItr;
  std::vector<float> priorities(queueFamily.queueCount);
  std::fill(priorities.begin(), priorities.end(), 0.0f);

  const auto queueIndex = (uint32_t)std::distance(bgnitr, queueFamilyItr);
  const DeviceQueueCreateInfo queueCreateInfo(
      {}, queueIndex, queueFamily.queueCount, priorities.data());

  const auto devextensions =
      physicalDevice.enumerateDeviceExtensionProperties();
  const auto devextEndItr = devextensions.end();
  const auto fndDevExtItr = std::find_if(
      devextensions.begin(), devextEndItr, [](ExtensionProperties prop) {
        return strcmp(prop.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0;
      });
  if (fndDevExtItr == devextEndItr)
    return main::Error::SWAPCHAIN_EXT_NOT_FOUND;

  const char *name = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
  const DeviceCreateInfo deviceCreateInfo({}, 1, &queueCreateInfo, 0, {}, 1,
                                          &name);
  this->device = this->physicalDevice.createDevice(deviceCreateInfo);

  queue = device.getQueue(queueFamilyIndex, queueIndex);

  const auto localError = createWindowAndGetSurface();
  if (localError != main::Error::NONE)
    return localError;

  if (!physicalDevice.getSurfaceSupportKHR(queueIndex, surface))
    return main::Error::NO_SURFACE_SUPPORT;

  const auto properties = physicalDevice.getProperties();
  const auto surfaceFormat = physicalDevice.getSurfaceFormatsKHR(surface);
  const auto surfEndItr = surfaceFormat.end();
  const auto fitr = std::find_if(
      surfaceFormat.begin(), surfEndItr, [](SurfaceFormatKHR format) {
        return format.format == Format::eB8G8R8A8Unorm;
      });
  if (fitr == surfEndItr)
    return main::Error::FORMAT_NOT_FOUND;
  format = *fitr;

  const auto memoryProperties = physicalDevice.getMemoryProperties();
  const auto memBeginItr = memoryProperties.memoryTypes.begin();
  const auto memEndItr = memoryProperties.memoryTypes.end();
  const auto deviceLocalFindItr =
      std::find_if(memBeginItr, memEndItr, [](MemoryType &type) {
        return type.propertyFlags & (MemoryPropertyFlagBits::eDeviceLocal);
      });
  memoryTypeDeviceLocal = std::distance(memBeginItr, deviceLocalFindItr);
  const auto hostVisibleFindItr =
      std::find_if(memBeginItr, memEndItr, [](MemoryType &type) {
        return type.propertyFlags & (MemoryPropertyFlagBits::eHostVisible |
                                     MemoryPropertyFlagBits::eHostCoherent);
      });
  memoryTypeHostVisibleCoherent =
      std::distance(memBeginItr, hostVisibleFindItr);

  const auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

  const auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
  const auto presentModesEndItr = presentModes.end();
  const auto fndPresentMode = std::find(
      presentModes.begin(), presentModesEndItr, PresentModeKHR::eMailbox);
  if (fndPresentMode == presentModesEndItr) // Maybe relax?
    return main::Error::PRESENTMODE_NOT_FOUND;
  const auto presentMode = *fndPresentMode;

  const SwapchainCreateInfoKHR swapchainCreateInfo(
      {}, surface, 3, format.format, format.colorSpace,
      capabilities.currentExtent, 1, ImageUsageFlagBits::eColorAttachment,
      SharingMode::eExclusive, 0, nullptr,
      SurfaceTransformFlagBitsKHR::eIdentity,
      CompositeAlphaFlagBitsKHR::eOpaque, presentMode, true, nullptr);

  swapchain = device.createSwapchainKHR(swapchainCreateInfo);

  images = device.getSwapchainImagesKHR(swapchain);

  const std::array attachments = {AttachmentDescription(
      {}, format.format, SampleCountFlagBits::e1, AttachmentLoadOp::eClear,
      AttachmentStoreOp::eStore, AttachmentLoadOp::eDontCare,
      AttachmentStoreOp::eDontCare, ImageLayout::eGeneral,
      ImageLayout::eColorAttachmentOptimal)};

  constexpr std::array colorAttachments = {
      AttachmentReference(0, ImageLayout::eColorAttachmentOptimal)};

  const std::array subpassDescriptions = {SubpassDescription(
      {}, PipelineBindPoint::eGraphics, 0, {},
      (uint32_t)colorAttachments.size(), colorAttachments.data())};

  const std::array subpassDependencies = {SubpassDependency(
      VK_SUBPASS_EXTERNAL, 0, PipelineStageFlagBits::eTopOfPipe,
      PipelineStageFlagBits::eAllGraphics, (AccessFlagBits)0,
      AccessFlagBits::eInputAttachmentRead |
          AccessFlagBits::eColorAttachmentRead |
          AccessFlagBits::eColorAttachmentWrite)};

  const RenderPassCreateInfo renderPassCreateInfo(
      {}, (uint32_t)attachments.size(), attachments.data(),
      (uint32_t)subpassDescriptions.size(), subpassDescriptions.data());
  renderpass = device.createRenderPass(renderPassCreateInfo);

  imageviews.reserve(images.size());
  for (auto im : images) {
    const ImageViewCreateInfo imageviewCreateInfo(
        {}, im, ImageViewType::e2D, format.format, {},
        ImageSubresourceRange(ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    const auto imview = device.createImageView(imageviewCreateInfo);
    imageviews.push_back(imview);

    const FramebufferCreateInfo framebufferCreateInfo(
        {}, renderpass, 1, &imview, capabilities.currentExtent.width,
        capabilities.currentExtent.height, 1);
    framebuffer.push_back(device.createFramebuffer(framebufferCreateInfo));
  }

  const CommandPoolCreateInfo commandPoolCreateInfo(
      CommandPoolCreateFlagBits::eResetCommandBuffer, queueIndex);
  pool = device.createCommandPool(commandPoolCreateInfo);

  const CommandBufferAllocateInfo cmdBufferAllocInfo(
      pool, CommandBufferLevel::ePrimary, (uint32_t)imageviews.size() + 1);
  cmdbuffer = device.allocateCommandBuffers(cmdBufferAllocInfo);

  main::error = gamegraphics.init();
  if (main::error != main::Error::NONE)
    return main::error;

  const Viewport viewport(0, 0, capabilities.currentExtent.width,
                          capabilities.currentExtent.height, 0, 1);

  const PipelineViewportStateCreateInfo pipelineViewportCreateInfo({}, 1,
                                                                   &viewport);

  for (auto &pipeline : pipelineCreateInfos) {
    pipeline.pViewportState = &pipelineViewportCreateInfo;
  }

  const auto piperesult =
      device.createGraphicsPipelines({}, pipelineCreateInfos);
  VERROR(piperesult.result);
  pipelines = piperesult.value;

  const SemaphoreCreateInfo semaphoreCreateInfo;
  waitSemaphore = device.createSemaphore(semaphoreCreateInfo);
  signalSemaphore = device.createSemaphore(semaphoreCreateInfo);

  return main::Error::NONE;
}

void VulkanGraphicsModule::tick(double time) {
  auto nextimage =
      device.acquireNextImageKHR(swapchain, INT64_MAX, waitSemaphore, {});
  VERROR(nextimage.result);

  const PipelineStageFlags stageFlag = PipelineStageFlagBits::eAllGraphics;
  const SubmitInfo submitInfo(1, &waitSemaphore, &stageFlag, 1,
                              &cmdbuffer[nextimage.value], 1, &signalSemaphore);
  queue.submit(submitInfo, {});

  const PresentInfoKHR presentInfo(1, &signalSemaphore, 1, &swapchain,
                                   &nextimage.value, nullptr);
  const Result result = queue.presentKHR(presentInfo);
  VERROR(result);
}

void VulkanGraphicsModule::destroy() {
  device.destroySemaphore(waitSemaphore);
  device.destroySemaphore(signalSemaphore);
  for (auto pipe : pipelines)
    device.destroyPipeline(pipe);
  for (auto shader : shaderModules)
    device.destroyShaderModule(shader);
  device.freeCommandBuffers(pool, cmdbuffer);
  device.destroyCommandPool(pool);
  for (auto framebuff : framebuffer)
    device.destroyFramebuffer(framebuff);
  for (auto imv : imageviews)
    device.destroyImageView(imv);
  device.destroyRenderPass(renderpass);
  device.destroySwapchainKHR(swapchain);
#ifdef WIN32
  DestroyWindow((HWND)(*this->window));
#endif // WIN32
  device.destroy();
  instance.destroySurfaceKHR(surface);
#ifdef DEBUG
  DispatchLoaderDynamic stat;
  stat.vkDestroyDebugUtilsMessengerEXT =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          instance, "vkDestroyDebugUtilsMessengerEXT");
  instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, stat);
#endif
  instance.destroy();
}

main::Module *getNewModule() { return new VulkanGraphicsModule(); }

} // namespace tge::graphics
