#pragma once

#include "common.hpp" // IWYU pragma: keep

#include <experimental/simd>

namespace tor {

template <simd_able ValueType, typename SimdCallable, typename Callable>
auto any_of(const std::vector<ValueType> &data, SimdCallable any_of_callable,
            Callable any_of_elementwise_callable) -> bool {
  using VectorType = std::experimental::native_simd<ValueType>;
  for (std::size_t idx = 0; idx + VectorType::size() <= data.size();
       idx += VectorType::size()) {
    // TODO: Can we safely use vector_aligned here?
    if (std::experimental::any_of(any_of_callable(
            VectorType{&data[idx], std::experimental::vector_aligned})))
      return true;
  }
  for (std::size_t idx = data.size() % VectorType::size(); idx < data.size();
       ++idx) {
    if (any_of_elementwise_callable(data[idx]))
      return true;
  }
  return false;
}

template <simd_able ValueType, typename GenericCallable>
auto any_of(const std::vector<ValueType> &data,
            GenericCallable any_of_generic_callable) -> bool {
  using VectorType = std::experimental::native_simd<ValueType>;
  auto divresult = std::div(static_cast<int>(data.size()),
                            static_cast<int>(VectorType::size()));
  const auto until = data.size() - divresult.rem;
  for (std::size_t idx = 0; idx <= until; idx += VectorType::size()) {
    // TODO: Can we safely use vector_aligned here?
    if (std::experimental::any_of(any_of_generic_callable(
            VectorType{&data[idx], std::experimental::vector_aligned})))
      return true;
  }
  for (std::size_t idx = until; idx < data.size(); ++idx) {
    if (any_of_generic_callable(data[idx]))
      return true;
  }
  return false;
}

} // namespace tor
