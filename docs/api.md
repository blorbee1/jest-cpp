# API Reference

## Namespaces

| Name | Description |
|------|-------------|
| [`jest`](#jest) | Public API for the jest-cpp testing framework. |
| [`detail`](#detail) |  |

## Macros

---

### describe

```cpp
#define describe(name) if (::jest::detail::DescribeScope JEST_CAT(jest_scope_, __COUNTER__){name}; true)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1136?:1136

Declares a name test suite.

Creates a scope in which tests and nested suites can be registered. Lifecycle hooks are associated with the current suite.

```cpp
describe("math") {
    ...
}
```

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` |  | Descriptive name of the suite. |

---

### it

```cpp
#define it(...) ::jest::detail::TestRegistrar{__VA_ARGS__} = [&]()
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1163?:1163

Declares a test case.

Registers a test case with the current test suite.

#### Supported syntax

A test with default timeout: 
```cpp
it("name") {
    ...
};
```

A test with a custom timeout: 
```cpp
it("name", 5000) {
    ...
};
```

**See also**: test

---

### test

```cpp
#define test(...) it(__VA_ARGS__)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1169?:1169

Alias for `it`.

**See also**: it

---

### beforeAll

```cpp
#define beforeAll ::jest::detail::HookRegistrar{&::jest::detail::Frame::before_all} = [&]()
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1181?:1181

Code that is ran before all the test cases in a describe block.

Must be ran BEFORE any test case is called.

#### Example

```cpp
beforeAll { counter = 0; };
```

---

### afterAll

```cpp
#define afterAll ::jest::detail::HookRegistrar{&::jest::detail::Frame::after_all} = [&]()
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1194?:1194

Code that is ran after every test case in a describe block.

Must be ran BEFORE any test case is called. `afterAll` will run AFTER the local variables in the describe's block has been deallocated.

#### Example

```cpp
afterAll { cleanup(); };
```

---

### beforeEach

```cpp
#define beforeEach ::jest::detail::HookRegistrar{&::jest::detail::Frame::before_each} = [&]()
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1206?:1206

Code that is ran before each test case in a describe block.

Must be ran BEFORE any test case is called.

#### Example

```cpp
beforeEach { counter = 0; };
```

---

### afterEach

```cpp
#define afterEach ::jest::detail::HookRegistrar{&::jest::detail::Frame::after_each} = [&]()
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1218?:1218

Code that is ran after each test case in a describe block.

Must be ran BEFORE any test case is called.

#### Example

```cpp
afterEach { counter = 0; };
```



## jest

Public API for the jest-cpp testing framework.

### Classes

| Name | Description |
|------|-------------|
| [`Regex`](#regex) | A regular expression that retains its original pattern. |

### Functions

| Return | Name | Description |
|--------|------|-------------|
| JEST_CPP_INLINE [`Regex`](#regex) | [`operator""_re`](#operator_re)  | Creates a [Regex](#regex) from a string literal. |
| `auto` | [`expect`](#expect)  | Creates an expectation for a value. |
| `std::vector< std::tuple< Ts... > >` | [`table`](#table)  | Creates a typed table of rows for parmeterized tests. |
| `void` | [`init`](#init) `inline` | Initializes the test runner for command line arguments. |
| `int` | [`run`](#run) `inline` | Prints the test summary and returns the test result. |

---

#### operator""_re

```cpp
JEST_CPP_INLINE Regex operator""_re(const char * s, std::size_t n)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L116?:116

Creates a [Regex](#regex) from a string literal.

#### Returns
A regex constructed from the literal.

```cpp
jest::Regex pattern = "hello.*"_re;
```

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `s` | `const char *` | Pointer to the literal's characters. |
| `n` | `std::size_t` | Number of characters in the literal |

---

#### expect

```cpp
template<class T> auto expect(T && value)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1017?:1017

Creates an expectation for a value.

The returned object provides matchers such as toBe(), toEqual(), toContain(), and toThrow().

String literals are converted to std;:string before being stored.

#### Returns
An expectation object

```cpp
expect(42).toBe(42);
expect("hello").toContain("ell");
```

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `T &&` | Value to test |

---

#### table

```cpp
template<class... Ts> std::vector< std::tuple< Ts... > > table(std::initializer_list< std::tuple< Ts... > > rows)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1043?:1043

Creates a typed table of rows for parmeterized tests.

Each row is a tuple. When passed to it.each(), its elements are supplied as callback arguments.

#### Returns
A vector containing the supplied rows.

```cpp
it.each(table<int, int, int>({
    {1, 1, 2}
    {1, 2, 3}
})("%d + %d = %d", [](int a, int b, int sum) {
    expect(a + b).toBe(sum);
});
```

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `rows` | `std::initializer_list< std::tuple< Ts... > >` | Initializer list of rows. |

---

#### init

`inline`

```cpp
inline void init(int argc, char ** argv)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1064?:1064

Initializes the test runner for command line arguments.

Call this function at the beginning of main(), before registering any suites or tests.

Supported options include:

* `-v`, `--verbose`: Print every test result.
* `-t`, `--testNamePattern <regex>`: Run tests matching a regular expression against their full names.
* `--timeout <ms>`: Set the default per-test timeout.
* `--no-color`: Disable colored output.
* `-h`, `--help`: Print usage information and exit.

:::note
Invalid arguments cause the process to exit with status of 2. 

:::

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `argc` | `int` | Argument count from main(). |
| `argv` | `char **` | Argument vector from main(). |

---

#### run

`inline`

```cpp
inline int run()
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1075?:1075

Prints the test summary and returns the test result.

#### Returns
0 if all tests passed, 1 if any test failed.

:::note
If [init()](#init) was not called, the runner uses default options and prints a warning. 

:::

### Variables

| Return | Name | Description |
|--------|------|-------------|
| `detail::TestApi` | [`it`](#it-1) `constexpr` | Registers and executes a test case. |
| `detail::TestApi` | [`test`](#test-1)  |  |
| `detail::DescribeApi` | [`describe`](#describe-1) `constexpr` | Defines a test suite or nested test group. |

---

#### it

`constexpr`

```cpp
detail::TestApi it {}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L989?:989

Registers and executes a test case.

Supports ordinary tests, parameterized tests through .each(), and tests that are expected to fail through .failing().

---

#### test

```cpp
detail::TestApi test {}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L989?:989

---

#### describe

`constexpr`

```cpp
detail::DescribeApi describe {}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L998?:998

Defines a test suite or nested test group.

Groups tests under a descriptive name. Suites can register lifecycle hooks and contain nested suites.

Supports parameterized suite definitions through .each().



## Regex

```cpp
#include <jest-cpp.hpp>
```

```cpp
struct Regex
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L92?:92

> **Inherits:** `regex`

A regular expression that retains its original pattern.

Inherits from std::regex and stores the source pattern for use in assertion failure messages.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`source`](#source)  |  |

---

#### source

```cpp
std::string source
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L94?:94

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Regex`](#regex-1) `explicit` | Constructs a regular expression. |

---

#### Regex

`explicit`

```cpp
explicit Regex(std::string src, std::regex::flag_type f = std::regex::ECMAScript)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L101?:101

Constructs a regular expression.

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `std::string` | Regular expression pattern |
| `f` | `std::regex::flag_type` | Regular expression syntax flags |



## detail

### Classes

| Name | Description |
|------|-------------|
| [`EachBinder`](#eachbinder) |  |
| [`DescribeEachBinder`](#describeeachbinder) |  |
| [`ExpectationFailed`](#expectationfailed) | Thrown to indicate that an expectation failed its conditions. |
| [`Matchers`](#matchers) |  |
| [`EachFn`](#eachfn) |  |
| [`FailingApi`](#failingapi) |  |
| [`TestApi`](#testapi) |  |
| [`DescribeApi`](#describeapi) |  |

### Typedefs

| Return | Name | Description |
|--------|------|-------------|
| `std::conditional_t< std::is_pointer_v< T > &&std::is_same_v< std::remove_cv_t< std::remove_pointer_t< T > >, char >, std::string, T >` | [`RowOf`](#rowof)  |  |

---

#### RowOf

```cpp
using RowOf = std::conditional_t< std::is_pointer_v< T > &&std::is_same_v< std::remove_cv_t< std::remove_pointer_t< T > >, char >, std::string, T >
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L942?:942



## EachBinder

```cpp
template<class Row>
class EachBinder
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L908?:908

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`EachBinder`](#eachbinder-1) `inline` |  |
| `void` | [`operator()`](#operator) `const` `inline` |  |

---

#### EachBinder

`inline`

```cpp
inline EachBinder(std::vector< Row > r, bool fail)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L914?:914

---

#### operator()

`const` `inline`

```cpp
template<class F> inline void operator()(const std::string & name, F && fn, int timeoutMs = -1) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L916?:916

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::vector< Row >` | [`rows`](#rows)  |  |
| `bool` | [`failing`](#failing)  |  |

---

#### rows

```cpp
std::vector< Row > rows
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L910?:910

---

#### failing

```cpp
bool failing
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L911?:911



## DescribeEachBinder

```cpp
template<class Row>
class DescribeEachBinder
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L925?:925

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`DescribeEachBinder`](#describeeachbinder-1) `inline` |  |
| `void` | [`operator()`](#operator-1) `const` `inline` |  |

---

#### DescribeEachBinder

`inline`

```cpp
inline DescribeEachBinder(std::vector< Row > r, bool)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L930?:930

---

#### operator()

`const` `inline`

```cpp
template<class F> inline void operator()(const std::string & name, F && fn, int = -1) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L932?:932

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::vector< Row >` | [`rows`](#rows-1)  |  |

---

#### rows

```cpp
std::vector< Row > rows
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L927?:927



## ExpectationFailed

```cpp
#include <jest-cpp.hpp>
```

```cpp
struct ExpectationFailed
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L132?:132

> **Inherits:** `runtime_error`

Thrown to indicate that an expectation failed its conditions.



## Matchers

```cpp
template<class T>
struct Matchers
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L283?:283

> **Inherits:** `MatcherBase`

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`toBe`](#tobe) `const` `inline` | Checks whether the actual value equals the expected value. |
| `void` | [`toEqual`](#toequal) `const` `inline` | Performs deep equality. |
| `void` | [`toBeTruthy`](#tobetruthy) `const` `inline` | Checks that the value is truthy. |
| `void` | [`toBeFalsy`](#tobefalsy) `const` `inline` | Checks that the value is falsy. |
| `void` | [`toBeDefined`](#tobedefined) `const` `inline` | Checks that the value is considered defined. |
| `void` | [`toBeUndefined`](#tobeundefined) `const` `inline` | Checks that the value is considered undefined. |
| `void` | [`toBeNull`](#tobenull) `const` `inline` | Alias for toBeUndefined. |
| `void` | [`toBeGreaterThan`](#tobegreaterthan) `const` `inline` | Checks whether actual > expected. |
| `void` | [`toBeGreaterThanOrEqual`](#tobegreaterthanorequal) `const` `inline` | Checks whether actual >= expected. |
| `void` | [`toBeLessThan`](#tobelessthan) `const` `inline` | Checks whether actual < expected. |
| `void` | [`toBeLessThanOrEqual`](#tobelessthanorequal) `const` `inline` | Checks whether actual <= expected. |
| `void` | [`toBeCloseTo`](#tobecloseto) `const` `inline` | Checks whether a floating point number is close to an expected value. |
| `void` | [`toContain`](#tocontain) `const` `inline` | Checks whether a string contains a substring or a range contains an item. |
| `void` | [`toHaveLength`](#tohavelength) `const` `inline` | Checks the length of a string or range. |
| `void` | [`toHaveLength`](#tohavelength-1) `const` `inline` |  |
| `void` | [`toHaveLength`](#tohavelength-2) `const` `inline` |  |
| `void` | [`toMatch`](#tomatch) `const` `inline` | Checks whether a string contains a match for a regular expression. |
| `void` | [`toMatch`](#tomatch-1) `const` `inline` | Checks whether a string contains a match for a standard regex. |
| `void` | [`toMatch`](#tomatch-2) `const` `inline` | Checks whether a string contains the specifided substring. |
| `void` | [`toMatch`](#tomatch-3) `const` `inline` |  |
| `void` | [`toThrow`](#tothrow) `const` `inline` | Checks whether a callable throws any exception. |
| `void` | [`toThrow`](#tothrow-1) `const` `inline` | Checks whether a callable throws an exception whose message contains a substring. |

---

#### toBe

`const` `inline`

```cpp
template<class U> inline void toBe(const U & expected) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L310?:310

Checks whether the actual value equals the expected value.

Uses Object.is like semantics for floating point values, treating NaN as equal to NaN and +0 != -0. Pointers are compared by address.

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `expected` | `const U &` | Value to compare against |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the assertion fails. |

---

#### toEqual

`const` `inline`

```cpp
template<class U> inline void toEqual(const U & expected) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L324?:324

Performs deep equality.

Deeply compares optionals by comparing their values, pointers are compared by value, so on.

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `expected` | `const U &` | Value to compare against |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the assertion fails |

---

#### toBeTruthy

`const` `inline`

```cpp
inline void toBeTruthy() const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L332?:332

Checks that the value is truthy.

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the value is falsy |

---

#### toBeFalsy

`const` `inline`

```cpp
inline void toBeFalsy() const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L338?:338

Checks that the value is falsy.

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the value is truthy |

---

#### toBeDefined

`const` `inline`

```cpp
inline void toBeDefined() const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L346?:346

Checks that the value is considered defined.

Defined values only matter with std::optional and pointers, every other value is always "defined"

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the value is not defined |

---

#### toBeUndefined

`const` `inline`

```cpp
inline void toBeUndefined() const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L356?:356

Checks that the value is considered undefined.

Defined values only matter with std::optional and pointers, every other value is always "defined" **See also**: toBeNull

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the value is defined |

---

#### toBeNull

`const` `inline`

```cpp
inline void toBeNull() const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L362?:362

Alias for toBeUndefined.

**See also**: toBeUndefined

---

#### toBeGreaterThan

`const` `inline`

```cpp
template<class U> inline void toBeGreaterThan(const U & expected) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L371?:371

Checks whether actual > expected.

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `expected` | `const U &` | Lower bound for the actual value. |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the comparison fails. |

---

#### toBeGreaterThanOrEqual

`const` `inline`

```cpp
template<class U> inline void toBeGreaterThanOrEqual(const U & expected) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L382?:382

Checks whether actual >= expected.

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `expected` | `const U &` | Lower bound for the actual value. |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the comparison fails. |

---

#### toBeLessThan

`const` `inline`

```cpp
template<class U> inline void toBeLessThan(const U & expected) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L393?:393

Checks whether actual < expected.

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `expected` | `const U &` | Upper bound for the actual value. |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the comparison fails. |

---

#### toBeLessThanOrEqual

`const` `inline`

```cpp
template<class U> inline void toBeLessThanOrEqual(const U & expected) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L404?:404

Checks whether actual <= expected.

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `expected` | `const U &` | Upper bound for the actual value. |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the comparison fails. |

---

#### toBeCloseTo

`const` `inline`

```cpp
inline void toBeCloseTo(double expected, int digits = 2) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L414?:414

Checks whether a floating point number is close to an expected value.

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `expected` | `double` | Expected numeric value. |
| `digits` | `int` | Number of decimal digits used to determine tolerance. |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the values are not sufficiently close. |

---

#### toContain

`const` `inline`

```cpp
template<class U> inline void toContain(const U & item) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L433?:433

Checks whether a string contains a substring or a range contains an item.

Ranges are anything that has std::begin/std::end defined

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `item` | `const U &` | Item or substring to search for. |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the item is not found. |

---

#### toHaveLength

`const` `inline`

```cpp
inline void toHaveLength(size_t n) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L451?:451

Checks the length of a string or range.

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `n` | `size_t` | Expected length. |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the length differs. |

---

#### toHaveLength

`const` `inline`

```cpp
inline void toHaveLength(unsigned int n) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L457?:457

---

#### toHaveLength

`const` `inline`

```cpp
inline void toHaveLength(int n) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L464?:464

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `std::runtime_error` | If the supplied integer length is negative |

---

#### toMatch

`const` `inline`

```cpp
inline void toMatch(const Regex & re) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L475?:475

Checks whether a string contains a match for a regular expression.

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `re` | const [`Regex`](#regex) & | Regular expression to search for. |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If no match is found. |

---

#### toMatch

`const` `inline`

```cpp
inline void toMatch(const std::regex & re) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L486?:486

Checks whether a string contains a match for a standard regex.

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `re` | `const std::regex &` | Regular expression to search for. |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If no match is found, |

---

#### toMatch

`const` `inline`

```cpp
inline void toMatch(const std::string & sub) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L497?:497

Checks whether a string contains the specifided substring.

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `sub` | `const std::string &` | Substring to find. |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the substring is not found. |

---

#### toMatch

`const` `inline`

```cpp
inline void toMatch(const char * sub) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L501?:501

---

#### toThrow

`const` `inline`

```cpp
inline void toThrow() const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L509?:509

Checks whether a callable throws any exception.

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the callable does not throw. |

---

#### toThrow

`const` `inline`

```cpp
inline void toThrow(const std::string & substr) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L524?:524

Checks whether a callable throws an exception whose message contains a substring.

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `substr` | `const std::string &` | Substring expected in the exception message. |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the callable does not throw a matching exception. |



## EachFn

```cpp
template<template< class > class Binder>
struct EachFn
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L945?:945

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`failing`](#failing-1)  |  |

---

#### failing

```cpp
bool failing
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L946?:946

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `auto` | [`operator()`](#operator-2) `const` `inline` `requires Range<R>` |  |
| `auto` | [`operator()`](#operator-3) `const` `inline` |  |

---

#### operator()

`const` `inline` `requires Range<R>`

```cpp
template<class R> inline auto operator()(const R & rows) const requires Range<R>
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L948?:948

---

#### operator()

`const` `inline`

```cpp
template<class T> inline auto operator()(std::initializer_list< T > rows) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L953?:953



## FailingApi

```cpp
struct FailingApi
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L959?:959

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `EachFn< EachBinder >` | [`each`](#each)  |  |

---

#### each

```cpp
EachFn< EachBinder > each {true}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L960?:960

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`operator()`](#operator-4) `const` `inline` |  |

---

#### operator()

`const` `inline`

```cpp
inline void operator()(const std::string & name, const std::function< void()> & fn, int timeoutMs = -1) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L961?:961



## TestApi

```cpp
struct TestApi
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L966?:966

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `EachFn< EachBinder >` | [`each`](#each-1)  |  |
| `FailingApi` | [`failing`](#failing-2)  |  |

---

#### each

```cpp
EachFn< EachBinder > each {false}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L967?:967

---

#### failing

```cpp
FailingApi failing {}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L968?:968

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`operator()`](#operator-5) `const` `inline` |  |

---

#### operator()

`const` `inline`

```cpp
inline void operator()(const std::string & name, const std::function< void()> & fn, int timeoutMs = -1) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L969?:969



## DescribeApi

```cpp
struct DescribeApi
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L974?:974

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `EachFn< DescribeEachBinder >` | [`each`](#each-2)  |  |

---

#### each

```cpp
EachFn< DescribeEachBinder > each {false}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L975?:975

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`operator()`](#operator-6) `const` `inline` |  |

---

#### operator()

`const` `inline`

```cpp
inline void operator()(const std::string & name, const std::function< void()> & fn) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L976?:976

Generated by [Moxygen](https://0state.com/moxygen)