#include "CellEntry.hpp"

namespace CellEntry {

std::array<std::vector<Cell>, MAX_DEGREE> cellsPerLayer;
std::array<std::vector<glm::vec4>, MAX_DEGREE> cellDataPerLayer;
std::array<uint32_t, MAX_DEGREE> indexCount;
std::array<std::vector<double>, MAX_DEGREE> polynomialCache;
std::array<std::vector<glm::vec3>, MAX_DEGREE> maxCache;

} // namespace CellEntry
