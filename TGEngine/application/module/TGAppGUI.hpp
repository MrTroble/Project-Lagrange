#pragma once

#include "../util/Calculations.hpp"
#include "../util/VulkanHelper.hpp"
#include <algorithm>
#include <graphics/GUIModule.hpp>
#include <imgui.h>

constexpr auto minY = -100.0f;
constexpr auto maxY = 100.0f;

constexpr auto minInterpolate = 1;
constexpr auto maxInterpolate = 10;

class TGAppGUI : public tge::gui::GUIModule {
public:
  float currentY = 0.5;
  int interpolation = 4;

  void renderGUI() {
    ImGui::Begin("test");
    ImGui::SliderFloat("Y: ", &currentY, minY, maxY);
    ImGui::SliderInt("Interpolation: ", &interpolation, minInterpolate,
                     maxInterpolate);
    if (ImGui::Button("Apply")) {
      currentY = std::clamp(currentY, minY, maxY);
      interpolation = std::clamp(interpolation, minInterpolate, maxInterpolate);
      makeVulkan();
    }
    ImGui::End();
  }
};
