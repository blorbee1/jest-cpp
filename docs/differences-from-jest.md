# Differences from Jest

jest-cpp follows Jest's names and behavior wherever C++ allows. These are the places where it differs.

| Jest | jest-cpp | Why |
|---|---|---|
| `expect(x).not.toBe(y)` | `expect(x).never.toBe(y)` | `not` is a reserved word in C++ |
| `it("x", () => { ... })` | `it("x") { ... };` | a trailing `;` is required after every `it` |
| `/regex/` | `"regex"_re` | regex literal |
| `toBe` uses `Object.is`, `toEqual` is recursive | `toBe` is shallow, `toEqual` is deep | the closest C++ equivalent of reference vs. structural equality |
| `null` and `undefined` | an empty `std::optional` or a null pointer | C++ has one kind of "nothing" |
| tests are collected, then run | tests run immediately, in source order | lets plain locals in a `describe` body work |
| async tests; timeouts interrupt a test | tests are synchronous; the timeout is checked when a test returns | a running C++ function can't be safely interrupted |

## toBe and toEqual

- **`toBe`** is shallow. Pointers and smart pointers compare by address. Numbers follow `Object.is`: `NaN` equals `NaN`, and `+0` is not equal to `-0`, everything else uses `operator==`
- **`toEqual`** is deep. It looks through std::optional, pointers, smart pointers, containers, tuples, and compares strings by content.

For plain values like `int` and `std::string` the two behave the same, because C++ has value semantics. See the [matcher reference](matchers.md).

# null and undefined

`toBeDefined()` passes when an `std::optional` has a value or a pointer is non-null. `toBeUndefined()` and `toBeNull()` both pass when an optional is empty or a pointer is null