# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Mock functions: `jest::fn<Sig>()` or `jest::fn(impl)`, can be configured with `mockReturnValue`, `mockreturnValueOnce`, `mockImplementation`, `mockImplementationOnce`, `mockClear`, `mockReset`, and `mockName`
- New matchers: `toHaveBeencalled`, `toHaveBeenCalledTimes`, `toHaveBeenCalledWith`, `toHaveBeenLastCalledWith`, `toHaveBeenNthCalledWith`, `toHaveReturned`, `toHaveReturnedTimes`, `toHaveReturnedWith`, `toHaveLastReturnedWith`, `toHaveNthReturnedWith`
- `jest::anything` to match any single argument
- Mock function tester
- Mock functions documentation and example

## [1.0.1] - 2026-10-27

### Changed
- Compile times: failure messages are now built only when an expectation fails (not on every `expect`), and tests run through a lightweight non owning callable reference instead of `std::function`
- CI workflow now runs for multiple compilers

### Added
- Optional separate compilation: define `JEST_CPP_SEPARATE` everywhere and `JEST_CPP_IMPLEMENTATION` in one `.cpp` file (`src/jest-cpp.cpp`) to compile the `std::regex` parts once. This makes simple test files compile about 2.5x faster. The header still works on its own without it.
- Documentation page on compile times

## [1.0.0] - 2026-10-26

### Added

- `describe("name") { ... }` and `it("name") { ... };` / `test(...)` block syntax, with an optional per-test timeout.
- Hooks: `beforeAll`, `afterAll`, `beforeEach`, `afterEach`.
- Tests run immediately in source order, so local variables declared in a `describe` body are usable from its tests.
- `it.each` / `test.each` / `describe.each` with tuple, flat, string and character rows, a `table<...>()` helper, and printf-style titles (`%s %d %i %f %j %o %p %# %$ %%`).
- `test.failing`, `it.failing` and `it.failing.each`.
- Matchers: `toBe`, `toEqual`, `toBeTruthy`, `toBeFalsy`, `toBeDefined`, `toBeUndefined`, `toBeNull`, `toBeGreaterThan(OrEqual)`, `toBeLessThan(OrEqual)`, `toBeCloseTo`, `toContain`, `toHaveLength`, `toMatch`, `toThrow`, and `.never` negation.
- `toBe` follows `Object.is` for floating point; `toEqual` compares deeply through optionals, pointers, containers and tuples.
- `std::optional` and pointer support (`toBeDefined`, `toBeUndefined`, `toBeNull`).
- `"..."_re` regex literal.
- Command line options: `--verbose`, `--testNamePattern`, `--timeout`, `--no-color`, `--help`.
- Quiet output by default: failures and a summary only.
- `JEST_NO_MACROS` to opt out of the block-syntax macros.
- Examples, the library's own tests, and CI.
- Documentation site

[Unreleased]: https://github.com/blorbee1/jest-cpp/compare/v1.0.1...HEAD
[1.0.1]: https://github.com/blorbee1/jest-cpp/releases/tag/v1.0.1
[1.0.0]: https://github.com/blorbee1/jest-cpp/releases/tag/v1.0.0