# Contributing to jest-cpp

Thanks for wanting to help! Bug reports, documentation fixes, new matchers and examples are all welcome.

## Ways to contribute

- **Report a bug**: open an issue with a minimal program that reproduces it, your compiler and version, and the output you expected vs. got.
- **Suggest a feature**: open an issue first for anything bigger than a small matcher, so we can agree on the design before you spend time on it. It helps to say what the equivalent looks like in Jest.
- **Improve the docs or examples**: always welcome, no issue needed.
- **Send code**: see below.

## How changes get in

The `master` branch is protected: every change goes through a pull request that must pass CI.

1. Fork the repository and branch from `master`: `git switch -c fix-tohavelength-message`.
2. Make a focused change. One logical change per pull request is much easier to review than several.
3. Build and test locally (below) and fix any warnings.
4. Update the docs, examples, tests and [CHANGELOG](CHANGELOG.md) if needed (checklist below).
5. Open a pull request and fill in the template. Explain *why*, not only *what*.
6. CI runs on every push to the pull request. All checks must be green before it can be merged.
7. A maintainer reviews it. Expect questions and requests for changes. Push follow-up commits to the same branch.

## Building and testing

You need a C++20 compiler and CMake 3.16 or newer.

```sh
cmake -S . -B build -DJEST_CPP_WARNINGS_AS_ERRORS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```

This builds and runs:

- `examples/`: every example, so the documentation can't silently go stale
- `tests/selftest.cpp`: the library tested with itself; everything in it must pass
- `tests/failing_demo.cpp`: contains deliberate failures

Without CMake, the strictest check is compiling a test with the same warnings CI uses:

```sh
g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror -I include tests/selftest.cpp -o selftest && ./selftest
```

**The code must compile without warnings** under these flags. If you see a warning that looks like a compiler false positive, say so in the pull request instead of silencing it silently.
This is to keep the output clean when others include the library.

## Design rules

Please keep these in mind; they are why the library is pleasant to adopt.

- **One header, no dependencies**: everything lives in `include/jest-cpp.hpp` and uses only the standard library.
- **C++20** is the baseline. Don't require anything newer.
- **Internals go in `jest::detail`.** Only things users are meant to call live in `jest`.
- **Keep the macro surface tiny.** Macros exist only for the block syntax, are defined at the very bottom of the header. Don't add new ones lightly.
- **Follow Jest's naming and behavior** when there is a C++ equivalent, and document every place where we deliberately differ (see "Differences from Jest" in the README).
- **Failure messages matter.** A new matcher should say what it expected and what it received.
- **Give helpful compile errors.** Prefer a `static_assert` with a message to a wall of template noise.

## Checklists

### Adding a matcher

- [ ] Implement it in `Matchers<T>` in `include/jest-cpp.hpp`, supporting `.never` through `check(...)`
- [ ] Document it in [`docs/matchers.md`](docs/matchers.md)
- [ ] Add passing cases (and `never` cases) to `tests/selftest.cpp`
- [ ] If the failure message is new, add a case to `tests/failing_demo.cpp` or check it by hand
- [ ] Add an entry to **Unreleased** in the CHANGELOG

### Any pull request

- [ ] Builds without warnings under the flags above
- [ ] `ctest` passes
- [ ] New behavior is covered by a test
- [ ] User visible changes are in the README / docs and the CHANGELOG
- [ ] If an example changed, it still builds and runs (CI checks this)

## Style

- Match the surrounding code.
- Comment only when needed.
- Examples should be short, runnable programs that teach one idea each.

## Maintainers: making a release

1. Make sure `master` is green.
2. In the CHANGELOG, rename **Unreleased** to the new version with today's date and add a fresh empty **Unreleased** section above it. Update the comparison links at the bottom.
3. Bump the version in `CMakeLists.txt` (`project(jest-cpp VERSION ...)`).
4. Commit these through a pull request, then tag the merge commit `vX.Y.Z` and publish a GitHub Release using the CHANGELOG section as the notes.

Versions follow [Semantic Versioning](https://semver.org/): a change to the public API that can break users' code is a major bump (or a minor one while the version is below 1.0).