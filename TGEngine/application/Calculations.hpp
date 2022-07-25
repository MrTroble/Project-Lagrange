#pragma once

#include "Polynomials.hpp"
#include <array>
#include <glm/glm.hpp>
#include <vector>

constexpr auto MAX_DEGREE = 7;

struct Cell {

  Cell() { this->polynomials.reserve(216); }
  glm::vec3 centerOfCell;
  glm::vec3 points[8];
  std::vector<glm::vec4> polynomials;
};

extern std::array<std::vector<Cell>, MAX_DEGREE> cellsPerLayer;
extern std::array<std::vector<glm::vec4>, MAX_DEGREE> cellDataPerLayer;
extern std::array<uint32_t, MAX_DEGREE> indexCount;

inline glm::vec4 calculateHeights(const Cell &cell, glm::vec2 position3,
                                  glm::vec2 position2, glm::vec2 position1,
                                  glm::vec2 position0) {
  return glm::vec4();
}

struct Patch {
  glm::vec3 positions[4];
};

std::array<std::vector<Patch>, MAX_DEGREE> patchLists;

inline size_t degreeXFromLayer(size_t layer) { return layer; } // future method
inline size_t degreeYFromLayer(size_t layer) { return layer; } // future method

inline void generatePatches() {
  for (size_t layer = 0; layer < cellsPerLayer.size(); layer++) {
    const auto degreeX = degreeXFromLayer(layer);
    const auto degreeY = degreeYFromLayer(layer);
    const auto &cells = cellsPerLayer[layer];
    const auto &patches = patchLists[layer];
    for (const auto &cell : cells) {
      std::vector<glm::vec4> cellPolynomials;
      cellPolynomials.reserve(cell.polynomials.size());
      for (const auto &polynom : cell.polynomials) {
        
      }
    }
  }
}

template <uint32_t degree>
inline void calculate(const std::vector<Cell> &inout) {
  const Polynomial<degree> poly;
}

inline void makeData(float currentY, int interpolations) {
  std::numeric_limits<float> flim;
  for (auto &c : cellDataPerLayer)
    c.clear();
  for (size_t i = 0; i < indexCount.size(); i++) {
    const auto &cLayer = cellsPerLayer[i];
    switch (i) {
    case 0:
      calculate<0>(cLayer);
      break;
    case 1:
      calculate<1>(cLayer);
      break;
    case 2:
      calculate<2>(cLayer);
      break;
    case 3:
      calculate<3>(cLayer);
      break;
    case 4:
      calculate<4>(cLayer);
      break;
    case 5:
      calculate<5>(cLayer);
      break;
    case 6:
      calculate<6>(cLayer);
      break;
    case 7:
      calculate<7>(cLayer);
      break;
    case 8:
      calculate<8>(cLayer);
      break;
    default:
      break;
    }
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

        data[start + x * 4 + 3] = glm::vec4(position3, heights[0], 1);
        data[start + x * 4 + 2] = glm::vec4(position2, heights[1], 1);
        data[start + x * 4 + 1] = glm::vec4(position1, heights[2], 1);
        data[start + x * 4 + 0] = glm::vec4(position0, heights[3], 1);
      }
    }
  }
}
