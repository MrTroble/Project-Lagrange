#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>

constexpr auto MAX_DEGREE = 7;

struct Cell {

  Cell() { this->polynomials.reserve(216); }
  glm::vec3 centerOfCell;
  glm::vec3 points[8];
  std::vector<glm::vec4> polynomials;
};

namespace CellEntry {

extern std::array<std::vector<Cell>, MAX_DEGREE> cellsPerLayer;
extern std::array<std::vector<glm::vec4>, MAX_DEGREE> cellDataPerLayer;
extern std::array<uint32_t, MAX_DEGREE> indexCount;
extern std::array<std::vector<double>, MAX_DEGREE> polynomialCache;
extern std::array<std::vector<glm::vec3>, MAX_DEGREE> maxCache;

} // namespace CellEntry
