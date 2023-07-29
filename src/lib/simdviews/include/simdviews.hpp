#pragma once

#include <algorithm>
#include <bits/ranges_base.h>
#include <cstddef>
#include <experimental/simd>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <vector>

#include "common.hpp"

namespace tor {

/// Concepts

template <typename R>
concept simd_able_range =
    std::ranges::contiguous_range<R> && requires(R range) {
      std::experimental::native_simd<
          std::decay_t<decltype(*std::ranges::begin(range))>>{};
    };

static_assert(simd_able_range<std::vector<double>>, "Should be good");
static_assert(simd_able_range<std::vector<int>>, "Should be good");
static_assert(simd_able_range<std::array<double, 17>>, "Should be good");
static_assert(simd_able_range<std::array<int, 17>>, "Should be good");
// static_assert(simd_able_range<std::vector<bool>>, "Should not be good");
// static_assert(simd_able_range<std::vector<std::vector<int>>>,
//               "Should not be good");

/// SIMD Iterator

template <simd_able T> struct simd_iterator {
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = std::experimental::native_simd<T>;
  using pointer = std::experimental::native_simd<T> *;   // or also value_type*
  using reference = std::experimental::native_simd<T> &; // or also value_type&
};

template <simd_able_range R>
  requires std::ranges::view<R>
class simd_unwrapped_view
    : public std::ranges::view_interface<simd_unwrapped_view<R>> {
private:
  R base_{};
  std::ranges::iterator_t<R> iter_{std::begin(base_)};

public:
  simd_unwrapped_view() = default;

  constexpr simd_unwrapped_view(R base)
      : base_(base), iter_(std::begin(base_)) {}

  constexpr auto base() const & -> R { return base_; }
  constexpr auto base() && -> R { return std::move(base_); }

  constexpr auto begin() const { return iter_; }
  constexpr auto end() const { return std::end(base_); }

  constexpr auto size() const
    requires std::ranges::sized_range<const R>
  {
    const auto s = std::ranges::size(base_);
    // prevent overflow
    if (s == 0U)
      return 0U;
    // ceiling division
    return 1U + ((s - 1U) / std::experimental::native_simd<double>::size());
  }
};

template <class R>
simd_unwrapped_view(R &&base)
    -> simd_unwrapped_view<std::ranges::views::all_t<R>>;

struct simd_unwrapped_range_adaptor {
} unwrappedView;

template <simd_able_range R>
constexpr auto operator|(R &&r, const simd_unwrapped_range_adaptor a) {
  return a(std::forward<R>(r));
}

} // namespace tor
