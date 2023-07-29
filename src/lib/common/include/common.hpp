#pragma once // IWYU pragma: always_keep

#include <experimental/simd>

namespace tor {

// IWYU pragma: always_keep
template <typename T>
concept simd_able = requires(T value) { std::experimental::native_simd<T>{}; };

} // namespace tor
