#include "staticbtree.hpp"

#include <cassert>
#include <experimental/simd>
#include <iostream>
#include <iterator>
#include <memory>
#include <random>
#include <ranges>

#include <benchmark/benchmark.h>

namespace {

using intv = std::experimental::native_simd<int>;

auto linear_lower_bound(std::vector<int> &data, int query)
    -> std::vector<int>::iterator {
  for (auto it = data.begin(); it != data.end(); ++it) {
    if (query <= *it) [[unlikely]]
      return it;
  }
  return data.end();
}

auto linear_simd_lower_bound(std::vector<int> &data, int query) -> int {
  for (std::size_t idx = 0; idx + intv::size() <= data.size();
       idx += intv::size()) {
    auto vec = intv{&data[idx], std::experimental::vector_aligned};
    auto mask = query <= vec;
    if (std::experimental::any_of(mask)) [[unlikely]] {
      return idx + std::experimental::find_first_set(mask);
    }
  }
  return -1;
}

} // namespace

class LowerBoundBenchmark : public benchmark::Fixture {
public:
  LowerBoundBenchmark() {
    std::random_device rnd{};
    engine_.seed(rnd());
  }

protected:
  void SetUp(benchmark::State &state) override {
    data_.clear();
    const auto vectorSize = static_cast<std::size_t>(state.range(0));
    data_.reserve(vectorSize);
    std::uniform_int_distribution<int> valueDistribution{0, 100000};
    for (std::size_t i = 0; i < vectorSize; ++i) {
      data_.push_back(valueDistribution(engine_));
    }
    std::ranges::sort(data_);
    btree_ = std::make_unique<tor::StaticBtree<int>>(data_);
  }

  std::minstd_rand engine_;
  std::vector<int> data_;
  std::unique_ptr<tor::StaticBtree<int>> btree_;
};

BENCHMARK_DEFINE_F(LowerBoundBenchmark, lower_bound)
(benchmark::State &state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(std::ranges::lower_bound(data_, 20000));
    benchmark::DoNotOptimize(std::ranges::lower_bound(data_, 40000));
    benchmark::DoNotOptimize(std::ranges::lower_bound(data_, 60000));
    benchmark::DoNotOptimize(std::ranges::lower_bound(data_, 80000));
  }
};

// BENCHMARK_DEFINE_F(LowerBoundBenchmark, lower_bound_linear_search)
// (benchmark::State &state) {
//   for (auto _ : state) {
//     benchmark::DoNotOptimize(linear_lower_bound(data_, 20000));
//     benchmark::DoNotOptimize(linear_lower_bound(data_, 40000));
//     benchmark::DoNotOptimize(linear_lower_bound(data_, 60000));
//     benchmark::DoNotOptimize(linear_lower_bound(data_, 80000));
//   }
// };

// BENCHMARK_DEFINE_F(LowerBoundBenchmark, lower_bound_linear_simd_search)
// (benchmark::State &state) {
//   for (auto _ : state) {
//     benchmark::DoNotOptimize(linear_simd_lower_bound(data_, 20000));
//     benchmark::DoNotOptimize(linear_simd_lower_bound(data_, 40000));
//     benchmark::DoNotOptimize(linear_simd_lower_bound(data_, 60000));
//     benchmark::DoNotOptimize(linear_simd_lower_bound(data_, 80000));
//   }
// };

BENCHMARK_DEFINE_F(LowerBoundBenchmark, lower_bound_static_b_tree)
(benchmark::State &state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(btree_->lowerBound(20000));
    benchmark::DoNotOptimize(btree_->lowerBound(40000));
    benchmark::DoNotOptimize(btree_->lowerBound(60000));
    benchmark::DoNotOptimize(btree_->lowerBound(80000));
  }
};

BENCHMARK_REGISTER_F(LowerBoundBenchmark, lower_bound)
    ->RangeMultiplier(2)
    ->Range(1 << 9, 1 << 24);
// BENCHMARK_REGISTER_F(LowerBoundBenchmark, lower_bound_linear_search)
//     ->RangeMultiplier(2)
//     ->Range(1 << 9, 1 << 24);
// BENCHMARK_REGISTER_F(LowerBoundBenchmark, lower_bound_linear_simd_search)
//     ->RangeMultiplier(2)
//     ->Range(1 << 9, 1 << 24);
BENCHMARK_REGISTER_F(LowerBoundBenchmark, lower_bound_static_b_tree)
    ->RangeMultiplier(2)
    ->Range(1 << 9, 1 << 24);

// Run benchmark
BENCHMARK_MAIN();
