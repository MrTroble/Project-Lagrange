#pragma once

#include "CellEntry.hpp"
#include "Polynomials.hpp"
#include <array>
#include <functional>
#include <glm/glm.hpp>
#include <vector>

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

inline std::array<std::vector<double>, MAX_DEGREE>
generateYCaches(const double y) {
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

template <class T = double>
struct CalculationInfo {
  std::function<T(T, int)> xFunc;
  std::function<T(T, int)> yFunc;
  std::vector<double> cache;
  std::tuple<size_t, size_t, size_t> dimensions;
};

template <typename Itr, class T = double>
inline std::vector<T> calculateHeight(const CalculationInfo<T> &calculationInfo,
                                      const Itr pPolynomials,
                                      const std::vector<glm::vec2> &positions) {
  const auto &xFunc = calculationInfo.xFunc;
  const auto &yFunc = calculationInfo.yFunc;
  const auto &cache = calculationInfo.cache;
  const auto [dX, dY, dZ] = calculationInfo.dimensions;
  const auto polynomials = pPolynomials;

  std::vector<T> heights;
  heights.reserve(positions.size());
  for (const auto position : positions) {
    T height{};
    for (size_t x = 0; x <= dX; x++) {
      for (size_t y = 0; y <= dY; y++) {
        for (size_t z = 0; z <= dZ; z++) {
          const auto alpha = ;
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
  std::numeric_limits<double> flim;
  for (auto &c : CellEntry::cellDataPerLayer)
    c.clear();
  const auto yCaches = generateYCaches(currentY);
  for (size_t i = 0; i < CellEntry::indexCount.size(); i++) {
    const auto &cLayer = CellEntry::cellsPerLayer[i];
    CalculationInfo calculationInfo{};
    calculationInfo.cache = yCaches[i];
    calculationInfo.dimensions = degreeFromLayer(i);
    const auto [dX, dY, dZ] = calculationInfo.dimensions;
    calculationInfo.xFunc = getFunction(dX);
    calculationInfo.yFunc = getFunction(dX);
    const auto countPerCell = dX * dY * dZ;
    const auto &cache = CellEntry::polynomialCache[i];
    const auto beginItr = std::cbegin(cache);
    for (size_t c = 0; c < cLayer.size(); c++) {
      auto maxY = flim.min();
      auto minY = flim.max();
      const auto &cell = cLayer[c];
      for (size_t z = 0; z < 8; z++) {
        const double current = cell.points[z].z;
        maxY = std::max(maxY, current);
        minY = std::min(minY, current);
      }
      if (!(maxY >= currentY && currentY >= minY))
        continue;
      const auto iter = beginItr + (c * countPerCell);
    }
  }
}
