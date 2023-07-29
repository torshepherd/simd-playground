# simd-playground

I'm really excited to get `std::simd` in the standard one day. This is a collection of implementations of SIMD-vectorized algorithms/ data structures using only the C++ standard library (no intrinsics) in an attempt to learn how to write more performant code (and that will actually make it through code review ;) ).

## building with CMake

To build this project, you need

- GCC >= 11 (need libstdc++ with std::experimental::simd)
- CMake

Then just run

`CC=/usr/bin/gcc-12 CXX=/usr/bin/g++-12 cmake --build build`

and this should build the benchmarks `lower_bound_benchmark`, etc.

I've written a small convenience script (`scripts/run-benchmarks`) that will run the benchmark and plot the result with Python. Uses GCC 12.
