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
  ASSERT_EQ(cell.centerOfCell, glm::vec3(0.375, 0.625, 0.625));
  ASSERT_EQ(cell.points[0], glm::vec3(0.5, 0.5, 0.5));
  ASSERT_EQ(cell.polynomials[0], glm::vec4(0.5, 0.5, 0.5, 0.639095));

  const auto &locals = CellEntry::localPositions[6];
}
