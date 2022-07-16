#pragma once

#include <IO/IOModule.hpp>
#include <glm/glm.hpp>

class TGAppIO : public tge::io::IOModule {
public:
  glm::vec3 translation = glm::vec3(0, 0, 0);
  glm::vec3 scale = glm::vec3(1, 1, 1);
  glm::quat rotation = glm::quat();
  glm::mat4 mvpMatrix;
  glm::mat4 view = glm::mat4(1);
  glm::mat4 projectionMatrix =
      glm::perspective(glm::radians(75.0f), 1.0f, 0.0001f, 10000.0f);

  TGAppIO() { projectionMatrix[1][1] *= -1; }

  void mouseEvent(const tge::io::MouseEvent event) override {
    if (event.pressed & 1) {
      view = glm::lookAt(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0),
                         glm::vec3(0, 1, 0));
    }
  }

  void keyboardEvent(const tge::io::KeyboardEvent event) override {}
};
