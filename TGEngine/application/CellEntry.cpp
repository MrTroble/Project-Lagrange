#include "CellEntry.hpp"

namespace CellEntry {

std::array<std::vector<Cell>, MAX_DEGREE> cellsPerLayer;
std::array<std::vector<glm::vec4>, MAX_DEGREE> cellDataPerLayer;
std::array<std::vector<double>, MAX_DEGREE> polynomialHeightCache;
std::array<std::vector<std::vector<glm::vec2>>, MAX_DEGREE> localPositions;
std::array<std::vector<std::vector<glm::vec2>>, MAX_DEGREE> globalPositions;

} // namespace CellEntry
