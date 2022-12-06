# lizard
[![Build and test](https://github.com/Krzmbrzl/lizard/actions/workflows/build.yml/badge.svg)](https://github.com/Krzmbrzl/lizard/actions/workflows/build.yml)

A quantum chemistry application used for the symbolic derivation and manipulation of equations based on second quantization.

## Building

### Requirements

In order to build `lizard`, your system will have to meet the following requirements:
- An ISO-Cpp17 conform C++ compiler (+ the corresponding standard library)
- Java Runtime Environment (JRE) v11 or more recent
- [cmake](https://cmake.org/) v3.18 or more recent

All other dependencies will be fetched automatically while running cmake (requires an active internet connection). These dependencies are
- [cmake-compiler-flags](https://github.com/Krzmbrzl/cmake-compiler-flags) (BSD-3-Clause)
- [antlr4](https://github.com/antlr/antlr4) (BSD-3-Clause)
- [GoogleTest](https://github.com/google/googletest) (BSD-3-Clause)
- [CLI11](https://github.com/CLIUtils/CLI11) (BSD-3-Clause)
- [fmt](https://github.com/fmtlib/fmt) (MIT)
- [spdlog](https://github.com/gabime/spdlog) (MIT)


### Build `lizard`

From the root of the source tree, execute the following commands:
1. `mkdir build`
2. `cd build`
3. `cmake ..` (potentially with desired arguments, see below)
4. `cmake --build .`

The built libraries can be found in `build/lib/` whereas all executables can be found in `build/bin/`.

### Testing

In order to execute the built test cases, enter the `build` directory and execute `ctest --output-on-failure`.

### Available build options

Build options have to be passed to cmake, when invoking it. They are passed in the form of `-D<option>=<value>`. Thus, a cmake invocation with options
might look like this: `cmake -DCMAKE_BUILD_TYPE="Release" -DLIBPERM_EXAMPLES=ON ..`

| **Option** | **Description** | **Default** |
| ---------- | --------------- | ----------- |
| `CMAKE_BUILD_TYPE` | The type of the build. Most common choices are either `Debug` or `Release` | `Release` |
| `LIZARD_LTO` | Whether to enable [link time optimization](http://johanengelen.github.io/ldc/2016/11/10/Link-Time-Optimization-LDC.html) (LTO) in `Release` builds | `ON`, if supported |
| `LIZARD_BUILD_TESTS` | Whether to build test cases | `ON` |
| `LIZARD_DISABLE_WARNINGS` | Whether to disable all warnings related to `libPerm` source files | `OFF` |
| `LIZARD_WARNINGS_AS_ERRORS` | Whether to treat compiler warnings as errors | `OFF` |
| `LIZARD_IWYU` | Enable the [include-what-you-use](https://include-what-you-use.org/) tool, if installed | `ON` |
| `LIZARD_LWYU` | Enable cmake's link-what-you-use tool, if available | `ON` |
| `LIZARD_CLANG_TIDY` | Enable [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) static analysis, if installed | `ON` |

