# jest-cpp

[![CI](https://github.com/blorbee1/jest-cpp/actions/workflows/ci.yml/badge.svg)](https://github.com/blorbee1/jest-cpp/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)

A single header unit testing library for C++20 with the syntax and feel of [Jest](https://jestjs.io/).

```cpp
#include <jest-cpp.hpp>
using namespace jest;

int main(int argc, char** argv) {
    init(argc, argv); // Not required to call but if you want to use CLI, it is

    describe("Counter") {
        Counter c;
        
        it("starts at zero") { 
            expect(c.value()).toBe(0); 
        };

        it.each({1, 2, 3})("adds %i", [&](int n) {
            for (int i = 0; i < n; ++i) c.inc();
            expect(c.value()).toBeGreaterThan(0);
        });
    }

    return run(); // will output a warning if 'init' isn't called
}
```

```text
$ ./tests --verbose
Counter
  PASS starts at zero
  PASS adds 1
  PASS adds 2
  PASS adds 3

Tests:  0 failed, 4 passed, 4 total
Time:   0.000 s
```

Without `--verbose` the output is quiet: only failures and the summary are printed.

## Installation

Requires **C++20** or later. Developed and tested with GCC 13, 14, and 15

**Copy the header.** Put [`include/jest-cpp.hpp`](include/jest-cpp.hpp) in your project and compile:

```sh
g++ -std=c++20 -I include tests.cpp -o tests && ./tests
```

**CMake (FetchContent).**

```cmake
include(FetchContent)
FetchContent_Declare(jest-cpp
    GIT_REPOSITORY https://github.com/blorbee1/jest-cpp.git
    GIT_TAG        v1.0.0)
FetchContent_MakeAvailable(jest-cpp)

add_executable(my_tests tests.cpp)
target_link_libraries(my_tests PRIVATE jest-cpp::jest-cpp)
```

## The shape of a test program

```cpp
#include <string>                 // your other headers first...
#include <jest-cpp.hpp>           // ...jest-cpp last (it defines `it` and `test` macros)
using namespace jest;

int main(int argc, char** argv) {
    init(argc, argv);             // 1. before the first describe/it
    describe("...") { ... }       // 2. tests run as they are reached
    return run();                 // 3. prints the summary, returns 0 (pass) or 1 (failures)
}
```

More in the [guide](docs/guide.md), and runnable programs in [`examples/`](examples/):

| Example | Shows |
|---|---|
| [`basics.cpp`](examples/basics.cpp) | `describe`, `it`, `expect` |
| [`hooks.cpp`](examples/hooks.cpp) | `beforeAll`, `beforeEach`, `afterEach`, `afterAll` |
| [`each_and_failing.cpp`](examples/each_and_failing.cpp) | `.each`, `describe.each`, `.failing` |
| [`matchers.cpp`](examples/matchers.cpp) | a tour of the matchers |

## Command line

```text
-v, --verbose                  print every test result, grouped by describe block
-t, --testNamePattern <regex>  only run tests whose full name matches the regex
    --timeout <ms>             default per-test timeout (default 5000)
    --no-color                 disable colored output (also honors NO_COLOR)
-h, --help                     show this help
```

A test's full name is its `describe` names followed by its own name, separated by spaces (`Counter starts at zero`).

Exit codes: `0` all tests passed, `1` at least one failed, `2` bad command line.

## Differences from Jest

| Jest | jest-cpp | Why |
|---|---|---|
| `expect(x).not.toBe(y)` | `expect(x).never.toBe(y)` | `not` is a reserved word in C++ |
| `it("x", () => { ... })` | `it("x") { ... };` | a trailing `;` is required after every `it` |
| `/regex/` | `"regex"_re` | regex literal |
| `toBe` is `Object.is`, `toEqual` is recursive | `toBe` is shallow (pointers by address, `NaN` equals `NaN`, `+0 != -0`); `toEqual` is deep (looks through optionals, pointers, containers) | the closest C++ equivalent of reference vs. structural equality |
| `null` and `undefined` | an empty `std::optional` or a null pointer; `toBeNull`, `toBeUndefined` mean the same | C++ has one kind of "nothing" |
| tests are collected, then run | tests run immediately, in source order | lets plain locals in a `describe` body work |
| async tests, timeouts that interrupt | tests are synchronous; the timeout is checked when a test returns | a running C++ function can't be safely interrupted |

## Contributing

Contributions are welcome. See [CONTRIBUTING.md](CONTRIBUTING.md). Release notes are in [CHANGELOG.md](CHANGELOG.md).

## License

[MIT](LICENSE)