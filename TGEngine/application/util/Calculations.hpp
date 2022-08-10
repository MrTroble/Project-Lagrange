#pragma once

#include "CellEntry.hpp"
#include "Polynomials.hpp"
#include <array>
#include <functional>
#include <glm/glm.hpp>
#include <unordered_map>
#include <unordered_set>
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
	const auto absY = std::abs(y);
	for (size_t id = 0; id < i; id++) {
		cache.push_back(function(absY, id));
	}
	return cache;
}

template <class T = double> struct CalculationInfo {
	std::function<T(T, int)> xFunc;
	std::function<T(T, int)> yFunc;
	std::vector<double> cache;
	std::tuple<size_t, size_t, size_t> dimensions;
	const T* pPolynomials = nullptr;
};

template <class T = double>
inline std::vector<T> calculateHeight(const CalculationInfo<T>& calculationInfo,
	const std::vector<glm::vec2>& positions) {
	const auto& xFunc = calculationInfo.xFunc;
	const auto& yFunc = calculationInfo.yFunc;
	const auto& cache = calculationInfo.cache;
	const auto [dX, dY, dZ] = calculationInfo.dimensions;
	const auto polynomials = calculationInfo.pPolynomials;
	if (polynomials == nullptr)
		throw std::runtime_error("no polynomials given!");

	std::vector<T> heights;
	heights.reserve(positions.size());
	for (const auto position : positions) {
		const auto absPosition = glm::abs(position);
		T height{};
		for (size_t z = 0; z < dZ; z++) {
			for (size_t y = 0; y < dY; y++) {
				for (size_t x = 0; x < dX; x++) {
					const auto offset = z * dX * dY + y * dX + x;
					const auto alpha = *(polynomials + offset);
					const auto one = xFunc(absPosition.x, x) *
						yFunc(absPosition.y, y) * cache[z];
					height += alpha * one;
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
interpolate(const CalculationInfo<T>& calculationInfo,
	const std::vector<InterpolateInfo<T>>& interpolations,
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
		const auto startIndex = i * points * points;
		for (size_t x = 0; x < parts; x++) {
			for (size_t y = 0; y < parts; y++) {
				const auto first = x + y * points + startIndex;
				const auto nextY = x + (y + 1) * points + startIndex;
				const auto position1 =
					glm::vec4(positions2D[first] + pivot, heights[first], 1);
				const auto position2 =
					glm::vec4(positions2D[first + 1] + pivot, heights[first + 1], 1);

				const auto position3 =
					glm::vec4(positions2D[nextY + 1] + pivot, heights[nextY + 1], 1);
				const auto position4 =
					glm::vec4(positions2D[nextY] + pivot, heights[nextY], 1);
				positions.push_back(position4);
				positions.push_back(position3);
				positions.push_back(position2);
				positions.push_back(position1);
			}
		}
	}
	return positions;
}

#ifdef DEBUG
namespace std {
	template <> struct hash<InterpolateInfo<>> {
		_NODISCARD size_t operator()(const InterpolateInfo<>& key) const noexcept {
			const auto h1 = std::hash<double>{}(key.point1.x);
			const auto h2 = std::hash<double>{}(key.point1.y);
			const auto h3 = std::hash<double>{}(key.point2.x);
			const auto h4 = std::hash<double>{}(key.point2.y);
			return ((h1 ^ (h2 << 1)) ^ (h3 << 1)) ^ (h4 << 1);
		}
	};
	template <> struct equal_to<InterpolateInfo<>> {
		_NODISCARD bool operator()(const InterpolateInfo<>& key1,
			const InterpolateInfo<>& key2) const noexcept {
			return key1.point1 == key2.point1 && key1.point2 == key2.point2;
		}
	};
} // namespace std
#endif

inline void makeData(const float currentY, const int interpolationCount) {
	std::numeric_limits<double> flim;
	for (auto& c : CellEntry::cellDataPerLayer)
		c.clear();
	for (size_t i = 1; i < CellEntry::cellsPerLayer.size(); i++) {
		const auto& cLayer = CellEntry::cellsPerLayer[i];
		if (cLayer.empty())
			continue;
		CalculationInfo calculationInfo{};
		calculationInfo.dimensions = degreeFromLayer(i);
		const auto [dX, dY, dZ] = calculationInfo.dimensions;
		calculationInfo.xFunc = getFunction(dX);
		calculationInfo.yFunc = getFunction(dY);
		const auto countPerCell = dX * dY * dZ;
		const auto& cache = CellEntry::polynomialHeightCache[i];
		const auto& locals = CellEntry::localPositions[i];
		auto& cellData = CellEntry::cellDataPerLayer[i];
#ifdef DEBUG
		std::unordered_set<InterpolateInfo<>> unorderedSet;
#endif // DEBUG
		for (size_t c = 0; c < cLayer.size(); c++) {
			auto maxY = flim.min();
			auto minY = flim.max();
			const auto& cell = cLayer[c];
			for (size_t z = 0; z < 8; z++) {
				const double current = cell.points[z].z;
				maxY = std::max(maxY, current);
				minY = std::min(minY, current);
			}
			if (!(maxY >= currentY && currentY >= minY))
				continue;
			const auto cellOffset = c * dX * dY;

			const auto localY = (currentY - minY) / (maxY - minY);
			calculationInfo.cache = generateYCaches(localY, i);
			calculationInfo.pPolynomials = cache.data() + countPerCell * c;

			const auto pivot = glm::vec2(cell.points[0]);
			std::vector<InterpolateInfo<>> interpolations;
			const auto partX = dX - 1;
			const auto partY = dY - 1;
			const auto setpX = partX + 1;
			interpolations.resize(partX * partY);
			for (size_t x = 0; x < partX; x++) {
				for (size_t y = 0; y < partY; y++) {
					const auto offset = x + y * setpX + cellOffset;
					const auto nextOffset = x + 1 + (y + 1) * setpX + cellOffset;
					const auto local1 = locals[offset];
					const auto local2 = locals[nextOffset];
					interpolations[x + y * partX] = { local1, local2 };
				}
			}
#ifdef DEBUG
			for (const auto& info : interpolations) {
				if (unorderedSet.contains(info)) {
					printf("C: %d\n", c);
				}
			}
			unorderedSet.insert(begin(interpolations), end(interpolations));
#endif // DEBUG

			const auto positionsOut = interpolate(calculationInfo, interpolations,
				pivot, interpolationCount);
			const auto start = cellData.size();
			cellData.resize(start + positionsOut.size());
			std::copy(begin(positionsOut), end(positionsOut),
				begin(cellData) + start);
		}
	}
}
