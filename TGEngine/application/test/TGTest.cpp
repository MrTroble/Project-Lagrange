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

  ASSERT_EQ(CellEntry::cellsPerLayer[3].size(), 1);

  CellEntry::reset();

  ASSERT_EQ(CellEntry::cellsPerLayer[3].size(), 0);

  ASSERT_NO_THROW(readData("degree5.dcplt"));
}
