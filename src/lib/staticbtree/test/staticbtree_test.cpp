#include "staticbtree.hpp"

#include <numeric>
#include <ranges>

#include <gtest/gtest.h>

// Build
TEST(StaticBTreeTest, TestBuilding) {
  tor::StaticBtree<int> test_tree = {
      {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17}};
  std::cout << "Test tree was built: " << test_tree << "\n";
  EXPECT_TRUE(true);
}

TEST(StaticBTreeTest, TestQuerying) {
  std::vector<int> testData{2000};
  std::iota(testData.begin(), testData.end(), 0);
  tor::StaticBtree<int> test_tree = {testData};
  // std::cout << "Test tree was built: " << test_tree << "\n";
  for (const auto testPoint : testData) {
    const auto expected = std::ranges::lower_bound(testData, testPoint);
    if (expected != testData.end()) {
      ASSERT_EQ(*expected, test_tree.lowerBound(testPoint));
    }
  }
}
