#pragma once

#include <IO/IOModule.hpp>
#include <glm/glm.hpp>
#include <graphics/GameGraphicsModule.hpp>
#include "Calculations.hpp"

class TGAppIO : public tge::io::IOModule {
public:
  glm::vec3 translation = glm::vec3(0, 0, 0);
  glm::vec3 scale = glm::vec3(1, 1, 1);
  glm::mat4 rotation;
  glm::mat4 mvpMatrix = glm::mat4(1);
  glm::mat4 view = glm::mat4(1);
  glm::vec2 total;
  glm::mat4 projectionMatrix =
      glm::perspective(glm::radians(65.0f), 1.0f, 0.001f, 1000.0f);
  uint32_t binding = UINT32_MAX;
  tge::graphics::APILayer *api;
  glm::vec2 last;

  TGAppIO() {
    projectionMatrix[1][1] *= -1;
    calculateMatrix();
  }

  void calculateMatrix() {
    mvpMatrix = projectionMatrix * view *
                (glm::translate(translation) * glm::scale(scale) * rotation);
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
      view = glm::lookAt(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0),
                         glm::vec3(0, 1, 0));
      total += (glm::vec2(event.x, event.y) - last) * 0.001f;
      rotation = glm::toMat4(glm::quat(total.x, 0, 1, 0) *
                             glm::quat(total.y, 1, 0, 0));
    } else if (event.pressed == tge::io::SCROLL) {
      constexpr float WEIGHT = 0.0002f;
      scale += event.x * WEIGHT;
      if (scale.x < 0) {
        scale = glm::vec3(0.01f, 0.01f, 0.01f);
      }
    }
    last = glm::vec2(event.x, event.y);
  }

  void keyboardEvent(const tge::io::KeyboardEvent event) override {}
};
