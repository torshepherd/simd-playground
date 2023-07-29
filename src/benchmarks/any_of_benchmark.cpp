#include <benchmark/benchmark.h>

#include <cassert>
#include <cstdlib>
#include <experimental/simd>
#include <iostream>
#include <iterator>
#include <random>
#include <ranges>

#include "simdalgorithm.hpp"

namespace {

using doublev = std::experimental::native_simd<double>;

} // namespace

class AnyOfBenchmark : public benchmark::Fixture {
public:
  AnyOfBenchmark() {
    std::random_device rnd{};
    engine_.seed(rnd());
  }

protected:
  void SetUp(benchmark::State &state) override {
    data_.clear();
    const auto vectorSize = static_cast<std::size_t>(state.range(0));
    data_.reserve(vectorSize);
    std::uniform_real_distribution<double> valueDistribution{0.0, 1.0};
    for (std::size_t i = 0; i < vectorSize; ++i) {
      data_.push_back(valueDistribution(engine_));
    }
    // std::ranges::sort(data_);
  }

  std::minstd_rand engine_;
  std::vector<double> data_;
};

BENCHMARK_DEFINE_F(AnyOfBenchmark, any_of)
(benchmark::State &state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(std::ranges::any_of(data_, [](const double val) {
      const auto diff = val - 0.2;
      return diff < 1e-6 && diff > -1e-6;
    }));
    benchmark::DoNotOptimize(std::ranges::any_of(data_, [](const double val) {
      const auto diff = val - 0.4;
      return diff < 1e-6 && diff > -1e-6;
    }));
    benchmark::DoNotOptimize(std::ranges::any_of(data_, [](const double val) {
      const auto diff = val - 0.6;
      return diff < 1e-6 && diff > -1e-6;
    }));
    benchmark::DoNotOptimize(std::ranges::any_of(data_, [](const double val) {
      const auto diff = val - 0.8;
      return diff < 1e-6 && diff > -1e-6;
    }));
  }
};

BENCHMARK_DEFINE_F(AnyOfBenchmark, any_of_simd)
(benchmark::State &state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(tor::any_of(data_, [](const auto val) {
      const auto diff = val - 0.2;
      return diff < 1e-6 && diff > -1e-6;
    }));
    benchmark::DoNotOptimize(tor::any_of(data_, [](const auto val) {
      const auto diff = val - 0.4;
      return diff < 1e-6 && diff > -1e-6;
    }));
    benchmark::DoNotOptimize(tor::any_of(data_, [](const auto val) {
      const auto diff = val - 0.6;
      return diff < 1e-6 && diff > -1e-6;
    }));
    benchmark::DoNotOptimize(tor::any_of(data_, [](const auto val) {
      const auto diff = val - 0.8;
      return diff < 1e-6 && diff > -1e-6;
    }));
  }
};

BENCHMARK_REGISTER_F(AnyOfBenchmark, any_of)
    ->RangeMultiplier(2)
    ->Range(1 << 5, 1 << 18);
BENCHMARK_REGISTER_F(AnyOfBenchmark, any_of_simd)
    ->RangeMultiplier(2)
    ->Range(1 << 5, 1 << 18);

// Run benchmark
BENCHMARK_MAIN();
