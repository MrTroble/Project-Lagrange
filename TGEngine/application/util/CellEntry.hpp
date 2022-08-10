#pragma once

#include <array>
#include <glm/glm.hpp>
#include <tuple>
#include <vector>

constexpr auto MAX_DEGREE = 7;

struct Cell {

  Cell() { this->polynomials.reserve(216); }
  glm::vec3 centerOfCell;
  glm::vec3 points[8];
  std::vector<glm::vec4> polynomials;
};

inline std::tuple<size_t, size_t, size_t> degreeFromLayer(const size_t layer) {
  return {layer, layer, layer};
} // future method

namespace CellEntry {

// Input data
extern std::array<std::vector<Cell>, MAX_DEGREE> cellsPerLayer;

extern std::array<std::vector<double>, MAX_DEGREE> polynomialHeightCache;
extern std::array<std::vector<glm::vec2>, MAX_DEGREE>
    localPositions;

// Const data
extern glm::vec2 minMax;

// Renderdata
extern std::array<std::vector<glm::vec4>, MAX_DEGREE> cellDataPerLayer;

inline void reset() {
  for (auto &toReset : cellsPerLayer)
    toReset.clear();
  for (auto &toReset : polynomialHeightCache)
    toReset.clear();
  for (auto &toReset : localPositions)
    toReset.clear();
  for (auto &toReset : cellDataPerLayer)
    toReset.clear();
}

} // namespace CellEntry
