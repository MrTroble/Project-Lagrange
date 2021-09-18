#pragma once

#include "../../public/graphics/GameShaderModule.hpp"
#include <vulkan/vulkan.hpp>

namespace tge::shader {

class VulkanShaderModule : public tge::shader::ShaderAPI {

public:
  explicit VulkanShaderModule(void *vgm) : vgm(vgm) {}

  void *vgm;
  std::vector<DescriptorPool> descPools;
  std::vector<PipelineLayout> pipeLayouts;
  std::vector<DescriptorSetLayout> setLayouts;
  std::vector<DescriptorSet> descSets;
  std::vector<tge::shader::VulkanShaderPipe *> shaderPipes;
  // Legacy support
  std::vector<std::vector<BindingInfo>> defaultbindings;

  ShaderPipe loadShaderPipeAndCompile(
      const std::vector<std::string> &shadernames) override;

  ShaderPipe createShaderPipe(const ShaderCreateInfo *shaderCreateInfo,
                              const size_t shaderCount) override;

  size_t createBindings(ShaderPipe pipe, const size_t count = 1) override;

  void bindData(const BindingInfo *info, const size_t count) override;

  void addToRender(const size_t bindingID, void *customData) override;

  void addToMaterial(const graphics::Material *material,
                             void *customData) override;

  void init() override;

  void destroy() override;
};

} // namespace tge::shader
