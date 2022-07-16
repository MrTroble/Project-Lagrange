#pragma once

#include <graphics/GUIModule.hpp>
#include <imgui.h>

class TGAppGUI : public tge::gui::GUIModule {
public:

  float currentY = 0;

  void renderGUI() {
    ImGui::Begin("test");
    ImGui::SliderFloat("Y:", &currentY, -10000, 10000);
    ImGui::End();
  }
};
