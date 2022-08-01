#include "CellEntry.hpp"
#include <array>
#include <cmath>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <regex>
#include <string>

inline void prepareData() {
  for (size_t i = 0; i < CellEntry::cellsPerLayer.size(); i++) {
    const auto &layer = CellEntry::cellsPerLayer[i];
    const auto [dx, dy, dz] = degreeFromLayer(i);
    const auto countPerCell = dx * dy * dz;
    auto &cache = CellEntry::polynomialHeightCache[i];
    const auto startID = cache.size();
    cache.resize(startID + countPerCell * layer.size());
    auto &localcache = CellEntry::localPositions[i];
    const auto localStart = localcache.size();
    localcache.resize(layer.size() * dx * dy + localStart);
    for (size_t c = 0; c < layer.size(); c++) {
      const auto &cell = layer[c];
      auto polynomials = cell.polynomials;
      const auto min = cell.points[0];
      const auto minVec = glm::vec4(glm::vec3(min), 0);

      for (size_t p = 0; p < polynomials.size(); p++) {
        polynomials[p] -= minVec;
      }

      const auto sIter = begin(polynomials);
      const auto eIter = end(polynomials);
      std::stable_sort(sIter, eIter,
                       [&](auto v1, auto v2) { return v1.x < v2.x; });
      std::stable_sort(sIter, eIter,
                       [&](auto v1, auto v2) { return v1.y < v2.y; });
      std::stable_sort(sIter, eIter,
                       [&](auto v1, auto v2) { return v1.z < v2.z; });

      for (size_t p = 0; p < polynomials.size(); p++) {
        const auto a = polynomials[p][3];
        const auto offset = startID + p + c * countPerCell;
        cache[offset] = a;
      }
      for (size_t x = 0; x < dx; x++) {
        for (size_t y = 0; y < dy; y++) {
          localcache[x + dx * y + localStart] =
              glm::vec2(polynomials[x + dx * y]);
        }
      }
    }
  }
}

inline void readData(const std::string &&input) {
  std::ifstream fstream(input, std::ios_base::binary);
  if (!fstream)
    throw std::runtime_error("Could not find file!");
  Cell cell;
  std::regex findNumbers = std::regex("([^;,]*)[;,]*");
  std::array<float, 4> numbers;
  uint8_t index = 0;
  while (fstream.good() && !fstream.eof()) {
    std::string line;
    std::getline(fstream, line);
    const char control = line[0];
    std::smatch matches;
    uint8_t numberIndex = 0;
    while (std::regex_search(line, matches, findNumbers,
                             std::regex_constants::match_any)) {
      if (numberIndex != 0) {
        const std::string numberString = matches[1].str();
        if (numberString.empty())
          break;
        const auto number = std::stof(numberString);
        numbers[numberIndex - 1] = number;
      }
      numberIndex++;
      line = matches.suffix().str();
    }
    switch (control) {
    case 'M':
      if (!cell.polynomials.empty()) {
        const uint32_t degree =
            std::round(std::pow(cell.polynomials.size(), 1 / 3.0f));
        CellEntry::cellsPerLayer[degree].push_back(cell);
      }
      index = 0;
      cell = {};
      cell.centerOfCell = glm::vec3(numbers[0], numbers[1], numbers[2]);
      break;
    case 'P':
      cell.points[index++] = glm::vec3(numbers[0], numbers[1], numbers[2]);
      break;
    case 'V':
      cell.polynomials.push_back(
          glm::vec4(numbers[0], numbers[1], numbers[2], numbers[3]));
      break;
    default:
      break;
    }
  }
  const uint32_t degree =
      std::round(std::pow(cell.polynomials.size(), 1 / 3.0f));
  CellEntry::cellsPerLayer[degree].push_back(cell);
  prepareData();
}