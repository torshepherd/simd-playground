#pragma once

#include "common.hpp" // IWYU pragma: keep

#include <experimental/simd>
#include <limits>
#include <ostream>
#include <vector>

namespace tor {

namespace details {
static constexpr std::size_t cache_line_size = 64;
} // namespace details

template <typename ElementType,
          std::size_t blockSize =
              details::cache_line_size / sizeof(ElementType)>
// using ElementType = int;
// constexpr auto blockSize = details::cache_line_size / sizeof(ElementType);
class StaticBtree {
public:
  constexpr StaticBtree(const std::vector<ElementType> &reference)
      : inputSize_{reference.size()},
        numberOfBlocks_{(inputSize_ + blockSize - 1) / blockSize},
        data_{numberOfBlocks_,
              block_type{std::numeric_limits<ElementType>::max()}} {
    build(reference);
  }

  auto lowerBound(ElementType query) -> ElementType {
    int k = 0;
    auto res = std::numeric_limits<ElementType>::max();
    while (k < numberOfBlocks_) {
      const auto invertedMask = query > data_[k];
      std::size_t idx = std::experimental::find_first_set(!invertedMask);
      if (idx < blockSize) {
        res = data_[k][idx];
      }
      k = go(k, idx);
    }
    return res;
  }

private:
  using block_type = std::experimental::fixed_size_simd<ElementType, blockSize>;
  std::size_t inputIndex_{};
  std::size_t inputSize_;
  std::size_t numberOfBlocks_;
  std::vector<block_type> data_;

  static inline constexpr auto go(const std::size_t k, const std::size_t i)
      -> std::size_t {
    return k * (blockSize + 1) + i + 1;
  }

  constexpr void build(const std::vector<ElementType> &input, int k = 0) {
    if (k < numberOfBlocks_) {
      for (int i = 0; i < blockSize; i++) {
        build(input, go(k, i));
        data_.at(k)[i] = (inputIndex_ < inputSize_
                              ? input.at(inputIndex_++)
                              : std::numeric_limits<ElementType>::max());
      }
      build(input, go(k, blockSize));
    }
  }

  template <typename e, std::size_t b>
  friend constexpr auto operator<<(std::ostream &os,
                                   const StaticBtree<e, b> &tree)
      -> std::ostream &;
};

template <typename ElementType, std::size_t blockSize>
[[nodiscard]] constexpr auto
operator<<(std::ostream &os, const StaticBtree<ElementType, blockSize> &tree)
    -> std::ostream & {
  if (tree.data_.empty()) {
    return os;
  }
  os << "[";
  for (const auto block : tree.data_) {
    os << "[" << block[0U];
    for (std::size_t i = 1U; i < block.size(); ++i) {
      os << ", " << block[i];
    }
    os << "]";
    if (std::experimental::any_of(block != tree.data_.back())) {
      os << ", ";
    }
  }
  os << "]";
  return os;
}

} // namespace tor
