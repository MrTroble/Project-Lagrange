#pragma once

#include "Polynomials.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <array>

constexpr auto MAX_DEGREE = 7;

struct Cell {

  Cell() { this->polynomials.reserve(216); }
  glm::vec3 centerOfCell;
  glm::vec3 points[8];
  std::vector<glm::vec4> polynomials;
  uint32_t degreeX = 0;
  uint32_t degreeY = 0;

  void calculatePolynomials() {
    degreeX = std::round(std::pow(polynomials.size(), 1 / 3.0f));
    degreeY = degreeX;
    for (const auto &polynomial : polynomials) {
    }
  }
};

extern std::array<std::vector<Cell>, MAX_DEGREE> cellsPerLayer;
extern std::array<std::vector<glm::vec4>, MAX_DEGREE> cellDataPerLayer;
extern std::array<uint32_t, MAX_DEGREE> indexCount;

template <class T = double> struct Entry {
  P0<T> po;
  P1<T> p1;
  P2<T> p2;
  P3<T> p3;
  P4<T> p4;
  P5<T> p5;
  P6<T> p6;
  P7<T> p7;
};
const Entry<double> entry;

inline glm::vec4 calculateHeights(const Cell &cell, glm::vec2 position3,
                                  glm::vec2 position2, glm::vec2 position1,
                                  glm::vec2 position0) {
  return glm::vec4();
}

inline void makeData(float currentY, int interpolations) {
  std::numeric_limits<float> flim;
  for (size_t i = 0; i < indexCount.size(); i++) {
    const auto &cLayer = cellsPerLayer[i];
    for (const auto &cell : cLayer) {
      glm::vec3 maxVec(flim.min(), flim.min(), flim.min());
      glm::vec3 minVec(flim.max(), flim.max(), flim.max());
      for (size_t z = 0; z < 8; z++) {
        for (size_t id = 0; id < 3; id++) {
          const auto current = cell.points[z][id];
          maxVec[id] = std::max(maxVec[id], current);
          minVec[id] = std::min(minVec[id], current);
        }
      }
      if (!(maxVec.y >= currentY && currentY >= minVec.y))
        continue;
      const auto maxSize = indexCount[i];
      auto &data = cellDataPerLayer[i];
      const auto start = data.size();
      data.resize(start + maxSize * 4);
      const double side = std::sqrt(maxSize);
      const glm::vec2 difference = glm::vec2(maxVec - minVec) / glm::vec2(side);
      for (size_t x = 0; x < maxSize; x++) {
        const double xInterpolation = (x % (int)side);
        const double yInterpolation = std::floor(x / side);
        const auto position0 =
            difference * glm::vec2(xInterpolation, yInterpolation) +
            glm::vec2(minVec);
        const auto position1 =
            glm::vec2(position0.x + difference.x, position0.y);
        const auto position2 = position0 + difference;
        const auto position3 =
            glm::vec2(position0.x, position0.y + difference.y);

        glm::vec4 heights =
            calculateHeights(cell, position3, position2, position1, position0);
        data[start + x * 4 + 3] = glm::vec4(position3, heights[0], 1);
        data[start + x * 4 + 2] = glm::vec4(position2, heights[1], 1);
        data[start + x * 4 + 1] = glm::vec4(position1, heights[2], 1);
        data[start + x * 4 + 0] = glm::vec4(position0, heights[3], 1);
      }
    }
  }
}
