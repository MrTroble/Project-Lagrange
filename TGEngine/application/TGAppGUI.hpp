#pragma once

#include <graphics/GUIModule.hpp>
#include <imgui.h>

class TGAppGUI : public tge::gui::GUIModule {
public:

  float currentY = 0;
  int interpolation = 4;

  void renderGUI() {
    ImGui::Begin("test");
    ImGui::SliderFloat("Y: ", &currentY, -100, 100);
    ImGui::SliderInt("Interpolation: ", &interpolation, 1, 10);
    if (ImGui::Button("Apply")) {

    }
    ImGui::End();
  }
};
