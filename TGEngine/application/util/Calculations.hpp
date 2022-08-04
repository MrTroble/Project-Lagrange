#pragma once

#include "CellEntry.hpp"
#include "Polynomials.hpp"
#include <array>
#include <functional>
#include <glm/glm.hpp>
#include <vector>

template <class T = double>
inline std::function<T(T, int)> getFunction(const size_t degree) {
  switch (degree - 1) {
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

inline std::vector<double> generateYCaches(const double y, const size_t i) {
  std::vector<double> cache;
  cache.reserve(MAX_DEGREE + 1);
  const auto function = getFunction(i);
  for (size_t id = 0; id < i; id++) {
    cache.push_back(function(y, id));
  }
  return cache;
}

template <class T = double> struct CalculationInfo {
  std::function<T(T, int)> xFunc;
  std::function<T(T, int)> yFunc;
  std::vector<double> cache;
  std::tuple<size_t, size_t, size_t> dimensions;
  const T *pPolynomials;
};

template <class T = double>
inline std::vector<T> calculateHeight(const CalculationInfo<T> &calculationInfo,
                                      const std::vector<glm::vec2> &positions) {
  const auto &xFunc = calculationInfo.xFunc;
  const auto &yFunc = calculationInfo.yFunc;
  const auto &cache = calculationInfo.cache;
  const auto [dX, dY, dZ] = calculationInfo.dimensions;
  const auto polynomials = calculationInfo.pPolynomials;

  std::vector<T> heights;
  heights.reserve(positions.size());
  for (const auto position : positions) {
    T height{};
    for (size_t x = 0; x < dX; x++) {
      for (size_t y = 0; y < dY; y++) {
        for (size_t z = 0; z < dZ; z++) {
          const auto alpha = *(polynomials + (z * dX * dY + y * dX + x));
          height +=
              alpha * xFunc(position.x, x) * yFunc(position.y, y) * cache[z];
        }
      }
    }
    heights.push_back(height);
  }
  return heights;
}

template <class T = double> struct InterpolateInfo {
  glm::vec2 point1;
  glm::vec2 point2;
};

template <class T = double>
inline std::vector<glm::vec4>
interpolate(const CalculationInfo<T> &calculationInfo,
            const std::vector<InterpolateInfo<T>> &interpolations,
            const glm::vec2 pivot, const size_t count) {
  std::vector<glm::vec4> positions;
  std::vector<glm::vec2> positions2D;
  const auto parts = count + 1;
  const auto points = parts + 1;
  positions2D.resize(points * points * interpolations.size());
  for (size_t i = 0; i < interpolations.size(); i++) {
    const auto interpolate = interpolations[i];
    const auto differnce = interpolate.point2 - interpolate.point1;
    const auto step = differnce / (float)parts;
    const auto startIndex = i * points * points;
    for (size_t x = 0; x < points; x++) {
      for (size_t y = 0; y < points; y++) {
        const auto offset = x + y * points + startIndex;
        positions2D[offset] = interpolate.point1 + step * glm::vec2(x, y);
      }
    }
  }

  const auto heights = calculateHeight<T>(calculationInfo, positions2D);
  positions.reserve(parts * parts * 4 * interpolations.size());
  for (size_t i = 0; i < interpolations.size(); i++) {
    const auto interpolate = interpolations[i];
    const auto startIndex = i * points * points;
    for (size_t x = 0; x < parts; x++) {
      for (size_t y = 0; y < parts; y++) {
        const auto first = x + y * parts + startIndex;
        const auto nextY = x + (y + 1) * parts + startIndex;
        const auto position1 =
            glm::vec4(positions2D[first] + pivot, heights[first], 0);
        const auto position2 =
            glm::vec4(positions2D[first + 1] + pivot, heights[first + 1], 0);

        const auto position3 =
            glm::vec4(positions2D[nextY + 1] + pivot, heights[nextY + 1], 0);
        const auto position4 =
            glm::vec4(positions2D[nextY + 1] + pivot, heights[nextY + 1], 0);
        positions.push_back(position1);
        positions.push_back(position2);
        positions.push_back(position3);
        positions.push_back(position4);
      }
    }
  }
  return positions;
}

inline void makeData(const float currentY, const int interpolationCount) {
  std::numeric_limits<double> flim;
  for (auto &c : CellEntry::cellDataPerLayer)
    c.clear();
  for (size_t i = 1; i < CellEntry::cellsPerLayer.size(); i++) {
    const auto &cLayer = CellEntry::cellsPerLayer[i];
    if (cLayer.empty())
      continue;
    CalculationInfo calculationInfo{};
    calculationInfo.dimensions = degreeFromLayer(i);
    const auto [dX, dY, dZ] = calculationInfo.dimensions;
    calculationInfo.xFunc = getFunction(dX);
    calculationInfo.yFunc = getFunction(dY);
    const auto countPerCell = dX * dY * dZ;
    const auto &cache = CellEntry::polynomialHeightCache[i];
    const auto &locals = CellEntry::localPositions[i];
    auto &cellData = CellEntry::cellDataPerLayer[i];
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
      const auto localY = (currentY - minY) / (maxY - minY);
      calculationInfo.cache = generateYCaches(localY, i);
      calculationInfo.pPolynomials = cache.data() + countPerCell * c;

      const auto pivot = glm::vec2(cell.points[0]);
      std::vector<InterpolateInfo<>> interpolations;
      interpolations.resize(dX * dY);
      for (size_t x = 0; x < dX - 1; x++) {
        for (size_t y = 0; y < dY - 1; y++) {
          const auto local1 = locals[x + y * dX];
          const auto local2 = locals[x + 1 + (y + 1) * dX];
          interpolations[x + y * dX] = {local1, local2};
        }
      }
      const auto positionsOut = interpolate(calculationInfo, interpolations,
                                            pivot, interpolationCount);
      const auto start = cellData.size();
      cellData.resize(start + positionsOut.size());
      std::copy(begin(positionsOut), end(positionsOut),
                begin(cellData) + start);
    }
  }
}
