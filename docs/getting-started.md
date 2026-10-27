# Getting Started

jest-cpp is a single header unit testing library for C++20 with the syntax similar to [Jest](https://jestjs.io/)

## Install

Requires **C++20** or later.

### Copy the header

Put [`jest-cpp.hpp`](https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp) anywhere in your include path.

```sh
g++ -std=c++20 -I include tests.cpp -o tests && ./tests
```

For faster compiles you can also copy [`jest-cpp.cpp`](https://github.com/blorbee1/jest-cpp/blob/master/src/jest-cpp.cpp) and compile it; see [Compile times](compile-times.md)

### CMake (FetchContent)

```cmake
include(FetchContent)
FetchContent_Declare(jest-cpp
    GIT_REPOSITORY https://github.com/blorbee1/jest-cpp.git
    GIT_TAG        v1.0.0)
FetchContent_MakeAvailable(jest-cpp)

add_executable(my_tests tests.cpp)
target_link_libraries(my_tests PRIVATE JestCpp::JestCpp)
```

## Your first test

```cpp
#include <jest-cpp.hpp>
using namespace jest;

int main(int argc, char** argv) {
    init(argc, argv); // This isn't 100% needed, only needed if you want command line options
    
    describe("math") {
        it("adds") {
            expect(1 + 1).toBe(2);
        };
        it("subtracts") {
            expect(5 - 3).toBe(2);
        };
    }
    
    // jest::run() will throw a warning if jest::init() was never called
    return run(); // This will return the exit code for your tests, 0 = all good, 1 = something failed, 2 = command error
}
```

Build and run it. By default only failures and a summary are printed:

```text
Tests:  0 failed, 2 passed, 2 total
Time:   0.000 s
```

If a test fails, you see which one and why:

```text
FAIL math > adds
    expect(received).toBe(3)
    Received: 2
    
Tests:  1 failed, 1 passed, 2 total
Time:   0.002 s
```

Run with `--verbose` to see every test, grouped by `describe` blocks:

```text
math
    FAIL adds
        expect(received).toBe(3)
        Received: 2
    PASS subtracts
    
Tests:  1 failed, 1 passed, 2 total
Time:   0.002 s
```

## Next steps

- [Writing tests](writing-tests.md): `describe`, `it`, and hooks
- [Table-driven tests](each.md): `it.each` and `describe.each`
- [Matchers](matchers.md): everything you can `expect`
- [Examples](examples.mdx): complete, runnable programs