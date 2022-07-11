#include <IO/IOModule.hpp>
#include <TGEngine.hpp>
#include <Util.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <graphics/GameGraphicsModule.hpp>
#include "TGAppGUI.hpp"

using namespace tge::main;
using namespace tge::graphics;

int main() {
  TGAppGUI* gui = new TGAppGUI;
  lateModules.push_back(gui);

  const auto initResult = init();
  if (initResult != Error::NONE) {
    printf("Error in init!");
    return -1;
  }

  auto api = getAPILayer();
  auto shader = api->getShaderAPI();

  const auto startResult = start();
  if (startResult != Error::NONE) {
    printf("Error in start!");
    return -1;
  }

  return (uint32_t)startResult;
}