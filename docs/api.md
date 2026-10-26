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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1048?:1048

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1075?:1075

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1081?:1081

Alias for `it`.

**See also**: it

---

### beforeAll

```cpp
#define beforeAll ::jest::detail::HookRegistrar{&::jest::detail::Frame::before_all} = [&]()
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1093?:1093

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1106?:1106

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1118?:1118

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1130?:1130

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
| [`Regex`](#regex) | [`operator""_re`](#operator_re) `inline` | Creates a [Regex](#regex) from a string literal. |
| `auto` | [`expect`](#expect)  | Creates an expectation for a value. |
| `std::vector< std::tuple< Ts... > >` | [`table`](#table)  | Creates a typed table of rows for parmeterized tests. |
| `void` | [`init`](#init) `inline` | Initializes the test runner for command line arguments. |
| `int` | [`run`](#run) `inline` | Prints the test summary and returns the test result. |

---

#### operator""_re

`inline`

```cpp
inline Regex operator""_re(const char * s, std::size_t n)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L92?:92

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L959?:959

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L985?:985

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1006?:1006

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1017?:1017

Prints the test summary and returns the test result.

#### Returns
0 if all tests passed, 1 if any test failed.

:::note
If [init()](#init) was not called, the runner uses default options and prints a warning. 

:::

### Variables

| Return | Name | Description |
|--------|------|-------------|
| `const detail::TestApi` | [`it`](#it-1)  | Registers and executes a test case. |
| `const detail::TestApi` | [`test`](#test-1)  |  |
| `const detail::DescribeApi` | [`describe`](#describe-1)  | Defines a test suite or nested test group. |

---

#### it

```cpp
const detail::TestApi it {}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L931?:931

Registers and executes a test case.

Supports ordinary tests, parameterized tests through .each(), and tests that are expected to fail through .failing().

---

#### test

```cpp
const detail::TestApi test {}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L931?:931

---

#### describe

```cpp
const detail::DescribeApi describe {}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L940?:940

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L68?:68

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L70?:70

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Regex`](#regex-1) `inline` `explicit` | Constructs a regular expression. |

---

#### Regex

`inline` `explicit`

```cpp
inline explicit Regex(std::string src, std::regex::flag_type f = std::regex::ECMAScript)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L77?:77

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L884?:884



## EachBinder

```cpp
template<class Row>
class EachBinder
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L850?:850

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L856?:856

---

#### operator()

`const` `inline`

```cpp
template<class F> inline void operator()(const std::string & name, F && fn, int timeoutMs = -1) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L858?:858

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L852?:852

---

#### failing

```cpp
bool failing
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L853?:853



## DescribeEachBinder

```cpp
template<class Row>
class DescribeEachBinder
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L867?:867

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L872?:872

---

#### operator()

`const` `inline`

```cpp
template<class F> inline void operator()(const std::string & name, F && fn, int = -1) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L874?:874

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::vector< Row >` | [`rows`](#rows-1)  |  |

---

#### rows

```cpp
std::vector< Row > rows
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L869?:869



## ExpectationFailed

```cpp
#include <jest-cpp.hpp>
```

```cpp
struct ExpectationFailed
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L102?:102

> **Inherits:** `runtime_error`

Thrown to indicate that an expectation failed its conditions.



## Matchers

```cpp
template<class T>
struct Matchers
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L243?:243

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `const T &` | [`actual`](#actual)  |  |
| `bool` | [`negate`](#negate)  |  |

---

#### actual

```cpp
const T & actual
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L244?:244

---

#### negate

```cpp
bool negate
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L245?:245

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Matchers`](#matchers-1) `inline` |  |
| `void` | [`check`](#check) `const` `inline` |  |
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

#### Matchers

`inline`

```cpp
inline Matchers(const T & act, bool neg)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L247?:247

---

#### check

`const` `inline`

```cpp
inline void check(bool pass, const std::string & matcher, const std::string & received) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L249?:249

---

#### toBe

`const` `inline`

```cpp
template<class U> inline void toBe(const U & expected) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L268?:268

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L282?:282

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L290?:290

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L296?:296

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L304?:304

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L314?:314

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L320?:320

Alias for toBeUndefined.

**See also**: toBeUndefined

---

#### toBeGreaterThan

`const` `inline`

```cpp
template<class U> inline void toBeGreaterThan(const U & expected) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L329?:329

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L340?:340

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L351?:351

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L362?:362

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L372?:372

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L391?:391

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L409?:409

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L415?:415

---

#### toHaveLength

`const` `inline`

```cpp
inline void toHaveLength(int n) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L422?:422

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L433?:433

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L444?:444

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L455?:455

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L459?:459

---

#### toThrow

`const` `inline`

```cpp
inline void toThrow() const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L467?:467

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L482?:482

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L887?:887

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`failing`](#failing-1)  |  |

---

#### failing

```cpp
bool failing
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L888?:888

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L890?:890

---

#### operator()

`const` `inline`

```cpp
template<class T> inline auto operator()(std::initializer_list< T > rows) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L895?:895



## FailingApi

```cpp
struct FailingApi
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L901?:901

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `EachFn< EachBinder >` | [`each`](#each)  |  |

---

#### each

```cpp
EachFn< EachBinder > each {true}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L902?:902

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L903?:903



## TestApi

```cpp
struct TestApi
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L908?:908

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L909?:909

---

#### failing

```cpp
FailingApi failing {}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L910?:910

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L911?:911



## DescribeApi

```cpp
struct DescribeApi
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L916?:916

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `EachFn< DescribeEachBinder >` | [`each`](#each-2)  |  |

---

#### each

```cpp
EachFn< DescribeEachBinder > each {false}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L917?:917

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L918?:918

Generated by [Moxygen](https://0state.com/moxygen)