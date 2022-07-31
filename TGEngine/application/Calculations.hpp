#pragma once

#include "CellEntry.hpp"
#include "Polynomials.hpp"
#include <array>
#include <functional>
#include <glm/glm.hpp>
#include <vector>

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

inline void getX(std::vector<glm::vec2> &data, uint32_t t) {}

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

template <class T = double> struct CalculationInfo {
  std::function<T(T, int)> xFunc;
  std::function<T(T, int)> yFunc;
  std::vector<double> cache;
  std::tuple<size_t, size_t, size_t> dimensions;
  T *pPolynomials;
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
    for (size_t x = 0; x <= dX; x++) {
      for (size_t y = 0; y <= dY; y++) {
        for (size_t z = 0; z <= dZ; z++) {
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

struct GlobalLocal {
  glm::vec4 global;
  glm::vec4 local;
};

class Storage {
  std::vector<GlobalLocal> data;
  glm::vec2 yExtent{};

  Storage(const std::vector<glm::vec4> &rawData) {
    data.resize(rawData.size());
    const auto pivot = glm::vec4(glm::vec3(rawData[0]), 0);
    for (size_t i = 0; i < rawData.size(); i++) {
      const auto position = rawData[i];
      data[i] = {position, position - pivot};
      yExtent.x = std::min(yExtent.x, position.z);
      yExtent.y = std::max(yExtent.y, position.z);
    }
  }
};

template <class T = double> struct InterpolateInfo {
  glm::vec2 pivot;
  glm::vec2 point1;
  glm::vec2 point2;
};

template <class T = double>
inline std::vector<glm::vec4>
interpolate(const CalculationInfo<T> &calculationInfo,
            const std::vector<InterpolateInfo<T>> &interpolations,
            const size_t count) {
  std::vector<glm::vec4> positions;
  std::vector<glm::vec2> positions2D;
  const auto parts = count + 1;
  const auto points = parts + 1;
  positions2D.resize(points * points *);
  for (size_t i = 0; i < interpolations.size(); i++) {
    const auto interpolate = interpolations[i];
    const auto differnce = interpolate.point2 - interpolate.point1;
    const auto step = differnce / (float)parts;
    const auto startIndex = positions.size();
    for (size_t x = 0; x < points; x++) {
      for (size_t y = 0; y < points; y++) {
        const auto first = interpolate.point1 + step * glm::vec2(x, y);
        positions[x + y * points + startIndex] = first;
      }
    }
  }
  const auto heights = calculateHeight<T>(calculationInfo, positions2D);
  for (size_t i = 0; i < interpolations.size(); i++) {
    for (size_t x = 0; x < points; x++) {
      for (size_t y = 0; y < points; y++) {
      }
    }
  }
  return positions;
}

inline void makeData(const float currentY, const int interpolations) {
  std::numeric_limits<double> flim;
  for (auto &c : CellEntry::cellDataPerLayer)
    c.clear();
  const auto yCaches = generateYCaches(currentY);
  for (size_t i = 0; i < CellEntry::cellsPerLayer.size(); i++) {
    const auto &cLayer = CellEntry::cellsPerLayer[i];
    CalculationInfo calculationInfo{};
    calculationInfo.cache = yCaches[i];
    calculationInfo.dimensions = degreeFromLayer(i);
    const auto [dX, dY, dZ] = calculationInfo.dimensions;
    calculationInfo.xFunc = getFunction(dX);
    calculationInfo.yFunc = getFunction(dX);
    const auto countPerCell = dX * dY * dZ;
    const auto &cache = CellEntry::polynomialHeightCache[i];
    const auto beginItr = std::cbegin(cache);
    auto &cellData = CellEntry::cellDataPerLayer[i];
    const auto startID = cellData.size();
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
    }
  }
}
