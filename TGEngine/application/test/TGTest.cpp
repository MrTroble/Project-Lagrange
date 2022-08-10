#include "../TGApp.hpp"
#include "../application/util/Calculations.hpp"
#include "../application/util/DataManager.hpp"
#include <gtest/gtest.h>

int main(int argv, char** in) {
	testing::InitGoogleTest(&argv, in);
	return RUN_ALL_TESTS();
}

TEST(data, input) {
	ASSERT_THROW(readData("dsjclskdjbcascnlkasjnc.doesNeverExist"),
		std::runtime_error);

	ASSERT_NO_THROW(readData("testInput.txt"));

	const auto& cells = CellEntry::cellsPerLayer[3];
	ASSERT_EQ(cells.size(), 1);
	const auto& cell = cells[0];
	ASSERT_EQ(cell.polynomials.size(), 27);

	CellEntry::reset();

	ASSERT_EQ(CellEntry::cellsPerLayer[3].size(), 0);

	ASSERT_NO_THROW(readData("degree5.dcplt"));

	ASSERT_FALSE(CellEntry::cellsPerLayer[6].empty());

	CellEntry::reset();
}

TEST(data, prepare) {
	ASSERT_NO_THROW(readData("degree5.dcplt"));
	const auto& cells = CellEntry::cellsPerLayer[6];
	ASSERT_EQ(cells.size(), 64);
	const auto& cell = cells[0];
	ASSERT_EQ(cell.polynomials.size(), 216);
	EXPECT_EQ(cell.centerOfCell, glm::vec3(0.375, 0.625, 0.625));
	EXPECT_EQ(cell.points[0], glm::vec3(0.5, 0.5, 0.5));
	EXPECT_EQ(cell.polynomials[0], glm::vec4(0.5, 0.5, 0.5, 0.639095));

	const auto& cell2 = cells[15];
	ASSERT_EQ(cell2.polynomials.size(), 216);
	EXPECT_EQ(cell2.centerOfCell, glm::vec3(0.625, 0.375, 0.625));
	EXPECT_EQ(cell2.points[0], glm::vec3(0.5, 0.5, 0.5));
	EXPECT_EQ(cell2.polynomials[0], glm::vec4(0.5, 0.5, 0.5, 0.646398));
	EXPECT_EQ(cell2.polynomials[5], glm::vec4(0.5, 0.25, 0.5, 0.191265));
	EXPECT_EQ(cell2.polynomials[11], glm::vec4(0.55, 0.25, 0.5, 0.607602));

	const auto& locals = CellEntry::localPositions[6];
	ASSERT_EQ(locals.size(), cells.size() * 6 * 6);
	EXPECT_EQ(locals[0], glm::vec2(-0.25, 0));
	EXPECT_EQ(locals[36], glm::vec2(0, 0));
	EXPECT_EQ(locals[37], glm::vec2(0.05, 0));
	EXPECT_EQ(locals[37], glm::vec2(0.05, 0));
	EXPECT_EQ(locals[36 * 15], glm::vec2(0, -0.25));
	const auto x = locals[36 * 15 + 1];
	EXPECT_FLOAT_EQ(x.x, 0.05f);
	EXPECT_FLOAT_EQ(x.y, -0.25f);

	const auto& cache = CellEntry::polynomialHeightCache[6];
	ASSERT_EQ(cache.size(), cells.size() * 6 * 6 * 6);
	EXPECT_FLOAT_EQ(cache[0], -0.0476528);
	EXPECT_FLOAT_EQ(cache[6 * 6 * 6], -0.000207452);
	EXPECT_FLOAT_EQ(cache[6 * 6 * 6 + 1], -0.00489827);

	CellEntry::reset();
}

TEST(maketest, ycache) {
	const auto& yCache = generateYCaches(0, 3);
	ASSERT_EQ(yCache.size(), 3);

	EXPECT_EQ(yCache[0], 1);
	EXPECT_EQ(yCache[1], 0);
	EXPECT_EQ(yCache[2], 0);

	const auto& yCache2 = generateYCaches(0.5, 3);
	ASSERT_EQ(yCache2.size(), 3);

	EXPECT_EQ(yCache2[0], 0);
	EXPECT_EQ(yCache2[1], 1);
	EXPECT_EQ(yCache2[2], 0);
}

TEST(maketest, interpolateTest) {
	CalculationInfo<> info;
	info.xFunc = getFunction(3);
	info.yFunc = getFunction(3);
	info.dimensions = degreeFromLayer(3);
	info.cache = generateYCaches(0, 3);
	std::array<double, 3 * 3 * 3> m;
	info.pPolynomials = m.data();

	std::vector<InterpolateInfo<>> interpolation = { {{0, 0}, {0.25, 0.25}} };
	const auto interp = interpolate(info, interpolation, glm::vec2(0, 0), 4);
	ASSERT_EQ(interp.size(), 5 * 5 * 4);
	EXPECT_EQ(glm::vec2(interp[3]), glm::vec2(0, 0));
	EXPECT_EQ(glm::vec2(interp[2]), glm::vec2(0.05, 0));
	EXPECT_EQ(glm::vec2(interp[1]), glm::vec2(0.05, 0.05));
	EXPECT_EQ(glm::vec2(interp[0]), glm::vec2(0, 0.05));

	EXPECT_EQ(glm::vec2(interp[7]), glm::vec2(0, 0.05));
	EXPECT_EQ(glm::vec2(interp[6]), glm::vec2(0.05, 0.05));
	EXPECT_EQ(glm::vec2(interp[5]), glm::vec2(0.05, 0.1));
	EXPECT_EQ(glm::vec2(interp[4]), glm::vec2(0, 0.1));

	constexpr auto offset = 5 * 4;
	EXPECT_EQ(glm::vec2(interp[offset + 3]), glm::vec2(0.05, 0));
	EXPECT_EQ(glm::vec2(interp[offset + 2]), glm::vec2(0.1, 0));
	EXPECT_EQ(glm::vec2(interp[offset + 1]), glm::vec2(0.1, 0.05));
	EXPECT_EQ(glm::vec2(interp[offset + 0]), glm::vec2(0.05, 0.05));
}

TEST(maketest, interpolateTestMultiple) {
	CalculationInfo<> info;
	info.xFunc = getFunction(3);
	info.yFunc = getFunction(3);
	info.dimensions = degreeFromLayer(3);
	info.cache = generateYCaches(0, 3);
	std::array<double, 3 * 3 * 3> m;
	std::fill(begin(m), end(m), 0);
	info.pPolynomials = m.data();

	std::vector<InterpolateInfo<>> interpolation = { {{0, 0}, {0.25, 0.25}},
													{{0, 1}, {0.25, 1.25}} };
	const auto interp = interpolate(info, interpolation, glm::vec2(0, 0), 4);
	ASSERT_EQ(interp.size(), 5 * 5 * 4 * interpolation.size());
	EXPECT_EQ(glm::vec2(interp[3]), glm::vec2(0, 0));
	EXPECT_EQ(glm::vec2(interp[2]), glm::vec2(0.05, 0));
	EXPECT_EQ(glm::vec2(interp[1]), glm::vec2(0.05, 0.05));
	EXPECT_EQ(glm::vec2(interp[0]), glm::vec2(0, 0.05));

	EXPECT_EQ(glm::vec2(interp[7]), glm::vec2(0, 0.05));
	EXPECT_EQ(glm::vec2(interp[6]), glm::vec2(0.05, 0.05));
	EXPECT_EQ(glm::vec2(interp[5]), glm::vec2(0.05, 0.1));
	EXPECT_EQ(glm::vec2(interp[4]), glm::vec2(0, 0.1));

	constexpr auto offset = 5 * 4;
	EXPECT_EQ(glm::vec2(interp[offset + 3]), glm::vec2(0.05, 0));
	EXPECT_EQ(glm::vec2(interp[offset + 2]), glm::vec2(0.1, 0));
	EXPECT_EQ(glm::vec2(interp[offset + 1]), glm::vec2(0.1, 0.05));
	EXPECT_EQ(glm::vec2(interp[offset + 0]), glm::vec2(0.05, 0.05));

	constexpr auto offset2 = 5 * 4 * 5;
	EXPECT_EQ(glm::vec2(interp[offset2 + 3]), glm::vec2(0, 1));
	EXPECT_EQ(glm::vec2(interp[offset2 + 2]), glm::vec2(0.05, 1));
	EXPECT_EQ(glm::vec2(interp[offset2 + 1]), glm::vec2(0.05, 1.05));
	EXPECT_EQ(glm::vec2(interp[offset2 + 0]), glm::vec2(0, 1.05));
}

TEST(maketest, interpolatePivotTest) {
	CalculationInfo<> info;
	info.xFunc = getFunction(3);
	info.yFunc = getFunction(3);
	info.dimensions = degreeFromLayer(3);
	info.cache = generateYCaches(0, 3);
	std::array<double, 3 * 3 * 3> m;
	info.pPolynomials = m.data();

	std::vector<InterpolateInfo<>> interpolation = { {{0, 0}, {0.25, 0.25}} };
	const auto interp = interpolate(info, interpolation, glm::vec2(1, 1), 4);
	ASSERT_EQ(interp.size(), 5 * 5 * 4);
	EXPECT_EQ(glm::vec2(interp[3]), glm::vec2(1, 1));
	EXPECT_EQ(glm::vec2(interp[2]), glm::vec2(1.05, 1));
	EXPECT_EQ(glm::vec2(interp[1]), glm::vec2(1.05, 1.05));
	EXPECT_EQ(glm::vec2(interp[0]), glm::vec2(1, 1.05));

	EXPECT_EQ(glm::vec2(interp[7]), glm::vec2(1, 1.05));
	EXPECT_EQ(glm::vec2(interp[6]), glm::vec2(1.05, 1.05));
	EXPECT_EQ(glm::vec2(interp[5]), glm::vec2(1.05, 1.1));
	EXPECT_EQ(glm::vec2(interp[4]), glm::vec2(1, 1.1));

	constexpr auto offset = 5 * 4;
	EXPECT_EQ(glm::vec2(interp[offset + 3]), glm::vec2(1.05, 1));
	EXPECT_EQ(glm::vec2(interp[offset + 2]), glm::vec2(1.1, 1));
	EXPECT_EQ(glm::vec2(interp[offset + 1]), glm::vec2(1.1, 1.05));
	EXPECT_EQ(glm::vec2(interp[offset + 0]), glm::vec2(1.05, 1.05));
}

TEST(maketest, sample1) {
	ASSERT_NO_THROW(readData("testInput.txt"));
	const auto& cells = CellEntry::cellsPerLayer[3];
	ASSERT_EQ(cells.size(), 1);

	const auto& cell = cells[0];
	ASSERT_EQ(cell.polynomials.size(), 3 * 3 * 3);

	const auto& locals = CellEntry::localPositions[3];
	ASSERT_EQ(locals.size(), cells.size() * 3 * 3);

	const auto& cache = CellEntry::polynomialHeightCache[3];
	ASSERT_EQ(cache.size(), cells.size() * 3 * 3 * 3);

	const auto& yCache = generateYCaches(0, 3);
	ASSERT_EQ(yCache.size(), 3);

	CalculationInfo<> info;
	info.xFunc = getFunction(3);
	info.yFunc = getFunction(3);
	info.dimensions = degreeFromLayer(3);
	const auto [dx, dy, dz] = info.dimensions;
	info.cache = yCache;
	info.pPolynomials = cache.data();
	const auto heights = calculateHeight(info, { {0, 0}, {0, 0.25} });
	ASSERT_EQ(heights.size(), 2);

	ASSERT_NO_THROW(makeData(0.5, 4));
	const auto& dataPerCell = CellEntry::cellDataPerLayer[3];
	ASSERT_EQ(dataPerCell.size(), 5 * 5 * 4 * 4 * cells.size());
	CellEntry::reset();
}

TEST(height, sample1) {
	ASSERT_NO_THROW(readData("testInput.txt"));

	CalculationInfo<> info;
	info.xFunc = getFunction(3);
	info.yFunc = getFunction(3);
	info.dimensions = degreeFromLayer(3);
	info.cache = generateYCaches(0, 3);
	info.pPolynomials = CellEntry::polynomialHeightCache[3].data();

	const auto interp =
		calculateHeight(info, {
		glm::vec2(-0.5, 0.5), glm::vec2(-0.5, 0.25), glm::vec2(-0.5, 0),//
		glm::vec2(-0.25, 0.5), glm::vec2(-0.25, 0.25), glm::vec2(-0.25, 0),//
		glm::vec2(0, 0.5), glm::vec2(0, 0.25), glm::vec2(0, 0) });
	EXPECT_FLOAT_EQ(interp[0], 0.410817);
	EXPECT_FLOAT_EQ(interp[1], 0.762032);
	EXPECT_FLOAT_EQ(interp[2], -0.227395);
	EXPECT_FLOAT_EQ(interp[3], -0.121273);
	EXPECT_FLOAT_EQ(interp[4], 0.120219);
	EXPECT_FLOAT_EQ(interp[5], 0.750364);
	EXPECT_FLOAT_EQ(interp[6], 0.219715);
	EXPECT_FLOAT_EQ(interp[7], -0.201654);
	EXPECT_FLOAT_EQ(interp[8], -0.388156);

	CellEntry::reset();
}
