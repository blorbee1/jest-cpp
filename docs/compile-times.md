# Compile times

A test file that includes `jest-cpp.hpp` compiles slower than a plain one, mostly because of `std::regex`, which is expensive to compile whenever it is actually used. There is an easy way to get compile times down.

At runtime, the code runs at an acceptable speed. This is just for compiling the code.

## Compile the expensive parts once

jest-cpp can build everything that touches `std::regex` a single time, instead of in every test file. How you do that depends on how you use the library.

### If you add jest-cpp to your build via FetchContent

This is automatic. When jest-cpp's own `CMakeLists.txt` runs, the `JestCpp::JestCpp` target adds a small static library and defines `JEST_CPP_SEPARATE` for you

To go back to a completely header only build while still using FetchContent, configure with `-DJEST_CPP_SEPARATE_COMPILATION=OFF`

### If you copied the files into your project

Copying `jest-cpp.hpp` and `jest-cpp.cpp` will still work but this will require you to compile `jest-cpp.cpp` yourself and define `JEST_CPP_SEPARATE` for every file that includes the header.

The simplest way is a small library that your test program links against. The definition is public, so it reaches your test files too:

```cmake
add_library(jest_cpp STATIC third_party/jest-cpp/jest-cpp.cpp)
target_include_directories(jest_cpp PUBLIC third_party/jest-cpp)
target_compile_features(jest_cpp PUBLIC cxx_std_20)
target_compile_definitions(jest_cpp PUBLIC JEST_CPP_SEPARATE)

add_executable(my_tests main.cpp math_tests.cpp)
target_link_libraries(my_tests PRIVATE jest_cpp)
```

**Without CMake:**

```sh
g++ -std=c++20 -I third_party/jest-cpp -DJEST_CPP_SEPARATE tests.cpp third_party/jest-cpp/jest-cpp.cpp -o tests
```

`jest-cpp.cpp` is a very small program and can just be written yourself:

```cpp
#define JEST_CPP_SEPARATE
#define JEST_CPP_IMPLEMENTATION
#include "jest-cpp.hpp"
```

`JEST_CPP_IMPLEMENTATION` is defined exactly in one .cpp file

If you define `JEST_CPP_SEPARATE` but forget the implementation file, you will receive linking errors

### Header only

If you still wish to have a header only library, nothing is needed for you to do. Without `JEST_CPP_SEPARATE` the header works on its own and everything is inline.