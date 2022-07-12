#include "TGAppGUI.hpp"
#include <IO/IOModule.hpp>
#include <TGEngine.hpp>
#include <Util.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <graphics/GameGraphicsModule.hpp>
#include <graphics/vulkan/VulkanShaderPipe.hpp>
#define SPR_NO_GLSL_INCLUDE 1
#define SPR_NO_STATIC 1
#define SPR_STATIC extern
#include <Util.hpp>
#include <headerlibs/ShaderPermute.hpp>

using namespace tge::main;
using namespace tge::graphics;
namespace permute {
std::map<std::string, int> lookupCounter;
permute::lookup glslLookup = {{"next", next}};
} // namespace permute

int main() {
  TGAppGUI *gui = new TGAppGUI;
  lateModules.push_back(gui);

  const auto initResult = init();
  if (initResult != Error::NONE) {
    printf("Error in init!");
    return -1;
  }

  auto api = getAPILayer();
  auto shader = api->getShaderAPI();

  glslang::InitializeProcess();
  tge::util::OnExit exitHandle(&glslang::FinalizeProcess);
  tge::shader::VulkanShaderPipe shaderPipe{};
  auto perm = permute::fromFile<permute::PermuteGLSL>(
      "assets/perInstanceVertexShader.vert");

  for (size_t i = 0; i < 6; i++) {
    permute::glslLookup["degree"] = [&](const auto &input) {
      return std::to_string(i);
    };
    permute::glslLookup["steps"] = permute::glslLookup["degree"];
    if (!perm.generate()) {
      for (auto &str : perm.getContent())
        printf(str.c_str());
      return -1;
    }
  }

  glslang::FinalizeProcess();

  const auto startResult = start();
  if (startResult != Error::NONE) {
    printf("Error in start!");
    return -1;
  }

  return (uint32_t)startResult;
}