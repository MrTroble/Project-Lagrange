#pragma once

#include <graphics/GUIModule.hpp>
#include <imgui.h>

class TGAppGUI : public tge::gui::GUIModule {

public:
  void renderGUI() {
    ImGui::Begin("test");
    ImGui::End();
  }
};
