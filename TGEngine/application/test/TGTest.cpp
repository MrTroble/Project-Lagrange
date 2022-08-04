#include "../application/util/Calculations.hpp"
#include "../application/util/DataManager.hpp"
#include <gtest/gtest.h>

int main(int argv, char **in) {
  testing::InitGoogleTest(&argv, in);
  return RUN_ALL_TESTS();
}

TEST(data, input) {
  ASSERT_THROW(readData("dsjclskdjbcascnlkasjnc.doesNeverExist"),
               std::runtime_error);

  ASSERT_NO_THROW(readData("testInput.txt"));

  const auto &cells = CellEntry::cellsPerLayer[3];
  ASSERT_EQ(cells.size(), 1);
  const auto &cell = cells[0];
  ASSERT_EQ(cell.polynomials.size(), 27);

  CellEntry::reset();

  ASSERT_EQ(CellEntry::cellsPerLayer[3].size(), 0);

  ASSERT_NO_THROW(readData("degree5.dcplt"));

  ASSERT_FALSE(CellEntry::cellsPerLayer[6].empty());

  CellEntry::reset();
}

TEST(data, prepare) {
  ASSERT_NO_THROW(readData("degree5.dcplt"));
  const auto &cells = CellEntry::cellsPerLayer[6];
  ASSERT_EQ(cells.size(), 64);
  const auto &cell = cells[0];
  ASSERT_EQ(cell.polynomials.size(), 216);
  EXPECT_EQ(cell.centerOfCell, glm::vec3(0.375, 0.625, 0.625));
  EXPECT_EQ(cell.points[0], glm::vec3(0.5, 0.5, 0.5));
  EXPECT_EQ(cell.polynomials[0], glm::vec4(0.5, 0.5, 0.5, 0.639095));

  const auto &locals = CellEntry::localPositions[6];
  ASSERT_EQ(locals.size(), cells.size() * 6 * 6);
  EXPECT_EQ(locals[0], glm::vec2(-0.25, 0));
  EXPECT_EQ(locals[36], glm::vec2(0, 0));
  EXPECT_EQ(locals[37], glm::vec2(0.05, 0));

  const auto &cache = CellEntry::polynomialHeightCache[6];
  ASSERT_EQ(cache.size(), cells.size() * 6 * 6 * 6);
  EXPECT_FLOAT_EQ(cache[0], -0.0476528);
  EXPECT_FLOAT_EQ(cache[6 * 6 * 6], -0.000207452);
  EXPECT_FLOAT_EQ(cache[6 * 6 * 6 + 1], -0.00489827);

  CellEntry::reset();
}

TEST(maketest, sample1) {
  ASSERT_NO_THROW(readData("testInput.txt"));
  const auto &cells = CellEntry::cellsPerLayer[3];
  ASSERT_EQ(cells.size(), 1);

  const auto &cell = cells[0];
  ASSERT_EQ(cell.polynomials.size(), 3 * 3 * 3);

  const auto &locals = CellEntry::localPositions[3];
  ASSERT_EQ(locals.size(), cells.size() * 3 * 3);

  const auto &cache = CellEntry::polynomialHeightCache[3];
  ASSERT_EQ(cache.size(), cells.size() * 3 * 3 * 3);

  const auto &yCache = generateYCaches(0, 3);
  ASSERT_EQ(yCache.size(), 3);

  EXPECT_EQ(yCache[0], 1);
  EXPECT_EQ(yCache[1], 0);
  EXPECT_EQ(yCache[2], 0);

  const auto &yCache2 = generateYCaches(0.5, 3);
  ASSERT_EQ(yCache2.size(), 3);

  EXPECT_EQ(yCache2[0], 0);
  EXPECT_EQ(yCache2[1], 1);
  EXPECT_EQ(yCache2[2], 0);

  CalculationInfo<> info;
  info.xFunc = getFunction(3);
  info.yFunc = getFunction(3);
  std::function<double(double, int)> referenceFunc =
      PolynomialEntry<>::P2functions;
  EXPECT_STREQ(info.xFunc.target_type().raw_name(),
               referenceFunc.target_type().raw_name());
  info.dimensions = degreeFromLayer(3);
  const auto [dx, dy, dz] = info.dimensions;
  info.cache = yCache;
  info.pPolynomials = cache.data();
  const auto heights = calculateHeight(info, {{0, 0}, {0, 0.25}});
  ASSERT_EQ(heights.size(), 2);
  double height1 = 0;
  double height2 = 0;
  for (size_t x = 0; x < dx; x++) {
    for (size_t y = 0; y < dy; y++) {
      for (size_t z = 0; z < dz; z++) {
        const auto a = cache[x + y * dx + z * dx * dy];
        height1 += a * info.xFunc(0, x) * info.xFunc(0, y) * info.xFunc(0, z);
        height2 +=
            a * info.xFunc(0, x) * info.xFunc(0.25, y) * info.xFunc(0, z);
      }
    }
  }
  EXPECT_FLOAT_EQ(heights[0], height1);
  EXPECT_FLOAT_EQ(heights[1], height2);

  std::vector<InterpolateInfo<>> interpolation = {{{0, 0}, {0.25, 0.25}}};
  const auto interp = interpolate(info, interpolation, glm::vec2(0, 0), 4);
  ASSERT_EQ(interp.size(), 5 * 5 * 4);
  EXPECT_EQ(glm::vec2(interp[0]), glm::vec2(0, 0));
  EXPECT_EQ(glm::vec2(interp[1]), glm::vec2(0.05, 0));
  EXPECT_EQ(glm::vec2(interp[2]), glm::vec2(0.05, 0.05));
  EXPECT_EQ(glm::vec2(interp[3]), glm::vec2(0, 0.05));

  EXPECT_EQ(glm::vec2(interp[4]), glm::vec2(0, 0.05));
  EXPECT_EQ(glm::vec2(interp[5]), glm::vec2(0.05, 0.05));
  EXPECT_EQ(glm::vec2(interp[6]), glm::vec2(0.05, 0.1));
  EXPECT_EQ(glm::vec2(interp[7]), glm::vec2(0, 0.1));

  ASSERT_NO_THROW(makeData(0.5, 4));
  const auto &dataPerCell = CellEntry::cellDataPerLayer[3];
  ASSERT_EQ(dataPerCell.size(), 5 * 5 * 4 * 4);
}
