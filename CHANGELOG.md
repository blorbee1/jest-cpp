# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

How to maintain it: every pull request that changes behavior adds a line under **Unreleased**, in the
right category (*Added*, *Changed*, *Deprecated*, *Removed*, *Fixed*, *Security*). When releasing, rename
**Unreleased** to the version and date and start a fresh empty **Unreleased** section above it.

## [Unreleased]

First public version (to be released as 1.0.0).

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

[Unreleased]: https://github.com/blorbee1/jest-cpp/commits/master