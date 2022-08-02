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
  ASSERT_EQ(cell.polynomials.size(), 3*3*3);

  const auto &locals = CellEntry::localPositions[3];
  ASSERT_EQ(locals.size(), cells.size() * 3 * 3);

  const auto &cache = CellEntry::polynomialHeightCache[3];
  ASSERT_EQ(cache.size(), cells.size() * 3 * 3 * 3);

  const auto &yCaches = generateYCaches(0);
  const auto &yCache = yCaches[3];
  ASSERT_EQ(yCache.size(), 3);

  ASSERT_EQ(yCache[0], 1);
  ASSERT_EQ(yCache[1], 0);
  ASSERT_EQ(yCache[2], 0);
}
