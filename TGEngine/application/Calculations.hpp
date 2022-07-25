#pragma once

#include "Polynomials.hpp"
#include <array>
#include <functional>
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

inline std::tuple<size_t, size_t, size_t> degreeFromLayer(const size_t layer) {
  return {layer, layer, layer};
} // future method

template <class T = double>
inline std::function<T(T, int)> getFunction(const size_t degree) {
  switch (degree) {
  case 0:
    return &PolynomialEntry<T>::P0functions;
  case 1:
    return &PolynomialEntry<T>::P1functions;
  case 2:
    return &PolynomialEntry<T>::P2functions;
  case 3:
    return &PolynomialEntry<T>::P3functions;
  case 4:
    return &PolynomialEntry<T>::P4functions;
  case 5:
    return &PolynomialEntry<T>::P5functions;
  case 6:
    return &PolynomialEntry<T>::P6functions;
  case 7:
    return &PolynomialEntry<T>::P7functions;
  case 8:
    return &PolynomialEntry<T>::P8functions;
  default:
    THROW("Unsupported degree!");
  }
}

inline std::array<std::vector<double>, MAX_DEGREE> generateYCaches(const double y) {
  std::array<std::vector<double>, MAX_DEGREE> yCaches;
  for (size_t i = 0; i < yCaches.size(); i++) {
    std::vector<double> cache;
    cache.reserve(MAX_DEGREE + 1);
    const auto function = getFunction(i);
    for (size_t id = 0; id <= i; id++) {
      cache.push_back(function(y, id));
    }
    yCaches[i] = cache;
  }
  return yCaches;
}

template <class T = double> struct CalculationInfo {
  std::function<T(T, int)> xFunc;
  std::function<T(T, int)> yFunc;
  std::vector<double> cache;
  std::tuple<size_t, size_t, size_t> dimensions;
  std::vector<glm::vec4> polynomials;
};

template <class T = double>
inline std::vector<T> calculateOne(const CalculationInfo<T> &calculationInfo,
                                   const std::vector<glm::vec2> positions) {
  const std::function<T(T, int)> &xFunc = calculationInfo.xFunc;
  const std::function<T(T, int)> &yFunc = calculationInfo.yFunc;
  const std::vector<double> &cache = calculationInfo.cache;
  const std::tuple<size_t, size_t, size_t> &dimensions =
      calculationInfo.dimensions;
  const std::vector<glm::vec4> &polynomials = calculationInfo.polynomials;

  std::vector<T> heights;
  heights.reserve(positions.size());
  for (const auto position : positions) {
    T height{};
    for (size_t x = 0; x <= std::get<0>(dimensions); x++) {
      for (size_t y = 0; y <= std::get<1>(dimensions); y++) {
        for (size_t z = 0; z <= std::get<2>(dimensions); z++) {
          const auto alpha =
              polynomials[x][3] + polynomials[y][3] + polynomials[z][3];
          height +=
              alpha * xFunc(position.x, x) * yFunc(position.y, y) * cache[z];
        }
      }
    }
    heights.push_back(height);
  }
  return heights;
}

inline void makeData(const float currentY, const int interpolations) {
  std::numeric_limits<float> flim;
  for (auto &c : cellDataPerLayer)
    c.clear();
  const auto yCaches = generateYCaches(currentY);
  for (size_t i = 0; i < indexCount.size(); i++) {
    const auto &cLayer = cellsPerLayer[i];
    CalculationInfo calculationInfo{};
    calculationInfo.cache = yCaches[i];
    calculationInfo.dimensions = degreeFromLayer(i);
    const auto [dX, dY, dZ] = calculationInfo.dimensions;
    calculationInfo.xFunc = getFunction(dX);
    calculationInfo.yFunc = getFunction(dX);
    for (const auto &cell : cLayer) {
      calculationInfo.polynomials = cell.polynomials;
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

        data[start + x * 4 + 3] = glm::vec4(position3, 0, 1);
        data[start + x * 4 + 2] = glm::vec4(position2, 0, 1);
        data[start + x * 4 + 1] = glm::vec4(position1, 0, 1);
        data[start + x * 4 + 0] = glm::vec4(position0, 0, 1);
      }
    }
  }
}
