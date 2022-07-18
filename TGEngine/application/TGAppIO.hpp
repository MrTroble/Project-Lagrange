#pragma once

#include <IO/IOModule.hpp>
#include <glm/glm.hpp>
#include <graphics/GameGraphicsModule.hpp>

class TGAppIO : public tge::io::IOModule {
public:
  glm::vec3 translation = glm::vec3(0, 0, 0);
  glm::vec3 scale = glm::vec3(1, 1, 1);
  glm::vec3 rotation = glm::vec3();
  glm::mat4 mvpMatrix = glm::mat4(1);
  glm::mat4 view = glm::mat4(1);
  glm::mat4 projectionMatrix =
      glm::perspective(glm::radians(65.0f), 1.0f, 0.001f, 1000.0f);
  uint32_t binding = UINT32_MAX;
  tge::graphics::APILayer *api;

  TGAppIO() {
    projectionMatrix[1][1] *= -1;
    calculateMatrix();
  }

  void calculateMatrix() {
    mvpMatrix = projectionMatrix * view *
                (glm::translate(translation) * glm::scale(scale) *
                 glm::mat4(1));
  }

  void sendChanges() {
    this->api->changeData(this->binding, &this->mvpMatrix,
                          sizeof(this->mvpMatrix));
  }

  void tick(double deltatime) override {
    calculateMatrix();
    sendChanges();
  }

  void mouseEvent(const tge::io::MouseEvent event) override {
    if ((event.pressed & 1) == 1) {
      rotation += glm::vec3(event.x, 0, event.y) * glm::vec3(0.001f, 0, 0.001f);
      view = glm::lookAt(rotation, glm::vec3(0, 0, 0),
                         glm::vec3(0, 1, 0));
    }
    if (event.pressed == tge::io::SCROLL) {
      constexpr float WEIGHT = 0.0002f;
      scale += event.x * WEIGHT;
      if (scale.x < 0) {
        scale = glm::vec3(0.01f, 0.01f, 0.01f);
      }
    }
  }

  void keyboardEvent(const tge::io::KeyboardEvent event) override {}
};
