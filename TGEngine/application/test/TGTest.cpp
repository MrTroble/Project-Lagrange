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
}
