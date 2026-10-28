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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1792?:1792

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1819?:1819

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1825?:1825

Alias for `it`.

**See also**: it

---

### beforeAll

```cpp
#define beforeAll ::jest::detail::HookRegistrar{&::jest::detail::Frame::before_all} = [&]()
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1837?:1837

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1850?:1850

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1862?:1862

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1874?:1874

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
| [`Mock< R(Args...)>`](#mockrargs) | A function that records how it was called. Create one with [`jest::fn`](#fn). |
| [`Anything`](#anything-1) | Placeholder that matches any single argument in the mock matchers. |
| [`Regex`](#regex) | A regular expression that retains its original pattern. |

### Enumerations

| Name | Description |
|------|-------------|
| [`ResultType`](#resulttype)  | What happened in one one of a mock. |

---

#### ResultType

```cpp
enum ResultType
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L96?:96

What happened in one one of a mock.

Found in `mock.results[i].type`

**See also**: Mock

| Value | Description |
|-------|-------------|
| `INCOMPLETE` |  |
| `RETURN` |  |
| `THROW` |  |
### Functions

| Return | Name | Description |
|--------|------|-------------|
| JEST_CPP_INLINE [`Regex`](#regex) | [`operator""_re`](#operator_re)  | Creates a [Regex](#regex) from a string literal. |
| `Mock< Sig >` | [`fn`](#fn)  | Create a new mock function with no implementation. |
| `Mock< Sig >` | [`fn`](#fn-1)  | Create a new mock function that runs an implementation function. |
| `auto` | [`fn`](#fn-2)  | Create a new mock function that runs an implementation function, taking the signature from it. |
| `auto` | [`expect`](#expect)  | Creates an expectation for a value. |
| `std::vector< std::tuple< Ts... > >` | [`table`](#table)  | Creates a typed table of rows for parmeterized tests. |
| `void` | [`init`](#init) `inline` | Initializes the test runner for command line arguments. |
| `int` | [`run`](#run) `inline` | Prints the test summary and returns the test result. |

---

#### operator""_re

```cpp
JEST_CPP_INLINE Regex operator""_re(const char * s, std::size_t n)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L149?:149

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

#### fn

```cpp
template<class Sig> Mock< Sig > fn()
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1582?:1582

Create a new mock function with no implementation.

Calling it records the call and returns a default value: `R{}` (nothing for `void`). Use mockReturnValue() or mockImplementation() to change what it does

#### Returns
The mock function

```cpp
auto charge = jest::fn<bool(double)>().mockReturnValue(true);
```

---

#### fn

```cpp
template<class Sig, class F> Mock< Sig > fn(F && impl)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1598?:1598

Create a new mock function that runs an implementation function.

#### Returns
The mock function

```cpp
auto half = jest::fn<double(int>([](int x) { return x / 2.0; });
```

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `impl` | `F &&` | What the mock does when it is called |

---

#### fn

```cpp
template<class F> auto fn(F && impl)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1616?:1616

Create a new mock function that runs an implementation function, taking the signature from it.

Works with lamdas that aren't generic and with function pointers

#### Returns
The mock function

```cpp
auto add jest::fn([](int a, int b) { return a + b; });
```

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `impl` | `F &&` | What the mock does when it is called |

---

#### expect

```cpp
template<class T> auto expect(T && value)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1665?:1665

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1691?:1691

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1712?:1712

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1723?:1723

Prints the test summary and returns the test result.

#### Returns
0 if all tests passed, 1 if any test failed.

:::note
If [init()](#init) was not called, the runner uses default options and prints a warning. 

:::

### Variables

| Return | Name | Description |
|--------|------|-------------|
| [`Anything`](#anything-1) | [`anything`](#anything) `constexpr` | Matches any single argument in the mock matchers. |
| `detail::TestApi` | [`it`](#it-1) `constexpr` | Registers and executes a test case. |
| `detail::TestApi` | [`test`](#test-1)  |  |
| `detail::DescribeApi` | [`describe`](#describe-1) `constexpr` | Defines a test suite or nested test group. |

---

#### anything

`constexpr`

```cpp
Anything anything {}
```

Type: [`Anything`](#anything-1)

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1630?:1630

Matches any single argument in the mock matchers.

Use it for the arguments you don't care about

```cpp
expect(log).toHaveBeenCalledWith(3, anything);
expect(log).toHaveBeenCalledWith(anything, "x");
```

---

#### it

`constexpr`

```cpp
detail::TestApi it {}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1637?:1637

Registers and executes a test case.

Supports ordinary tests, parameterized tests through .each(), and tests that are expected to fail through .failing().

---

#### test

```cpp
detail::TestApi test {}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1637?:1637

---

#### describe

`constexpr`

```cpp
detail::DescribeApi describe {}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1646?:1646

Defines a test suite or nested test group.

Groups tests under a descriptive name. Suites can register lifecycle hooks and contain nested suites.

Supports parameterized suite definitions through .each().



## Mock< R(Args...)>

```cpp
#include <jest-cpp.hpp>
```

```cpp
template<class R, class... Args>
class Mock< R(Args...)>
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1386?:1386

A function that records how it was called. Create one with [`jest::fn`](#fn).

A Mock is a cheap handle onto a shared state, copying it (for example into a std::function) keeps recording into the same place, so the original handle sees every call

Not thread-safe

#### Template Parameters
* `R` Type of the return value 

* `Args` Type of the arguments

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `const Data &` | [`mock`](#mock)  | The record of everything that happened to this mock. |

---

#### mock

```cpp
const Data & mock
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1430?:1430

The record of everything that happened to this mock.

* `mock.calls`: one tuple of arguments per call, in order
* `mock.results`: one entry per call, in order, with
  * `type`: a [ResultType](#resulttype)
  * `value`: a `std::optional` holding the return value (empty for `void`)
  * `error`: the `std::exception_ptr` of the exception, if the call threw

```cpp
auto m = jest::fn<int(int)>().mockReturnValue(7);
m(5);
std::get<0>(m.mock.calls[0]); // 5
m.mock.results[0].value; // Optional(7)
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `R` | [`operator()`](#operator) `const` `inline` | Calls the mock, records the call, then runs the next once-implementation, or the implementation, or returns `R{}` (or throws if `R` has no default constructor. |
| `Mock` | [`mockImplementation`](#mockimplementation) `const` `inline` | Sets what the mock does from now on. |
| `Mock` | [`mockImplementationOnce`](#mockimplementationonce) `const` `inline` | Sets what the next call does (once). Several can be queued, they are used in order. |
| `Mock` | [`mockReturnValue`](#mockreturnvalue) `const` `inline` `requires (!std::is_void_v<V>)` | Makes the mock return `v` from now on. |
| `Mock` | [`mockReturnValueOnce`](#mockreturnvalueonce) `const` `inline` `requires (!std::is_void_v<V>)` | Makes the mock return `v` (once). Several can be queued, they are used in order. |
| `Mock` | [`mockClear`](#mockclear) `const` `inline` | Forgets the recorded calls and results, keeps the implementations. |
| `Mock` | [`mockReset`](#mockreset) `const` `inline` | Calls `mockClear()` but also goes back to the implementation give to [jest::fn](#fn) (or to none). |
| `Mock` | [`mockName`](#mockname) `const` `inline` | Names the mock in failure messages (defaults to "jest.fn()"). |

---

#### operator()

`const` `inline`

```cpp
inline R operator()(Args... args) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1450?:1450

Calls the mock, records the call, then runs the next once-implementation, or the implementation, or returns `R{}` (or throws if `R` has no default constructor.

#### Returns
The value that got returned from the call, if not void

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `args` | `Args...` | The arguments to pass |

---

#### mockImplementation

`const` `inline`

```cpp
inline Mock mockImplementation(Impl f) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1488?:1488

Sets what the mock does from now on.

#### Returns
Itself

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `f` | `Impl` | Function to call |

---

#### mockImplementationOnce

`const` `inline`

```cpp
inline Mock mockImplementationOnce(Impl f) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1499?:1499

Sets what the next call does (once). Several can be queued, they are used in order.

#### Returns
Itself

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `f` | `Impl` | Function to call |

---

#### mockReturnValue

`const` `inline` `requires (!std::is_void_v<V>)`

```cpp
template<class V = R> inline Mock mockReturnValue(std::type_identity_t< V > v) const requires (!std::is_void_v<V>)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1512?:1512

Makes the mock return `v` from now on.

#### Returns
Itself

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `v` | `std::type_identity_t< V >` | Value to return |

---

#### mockReturnValueOnce

`const` `inline` `requires (!std::is_void_v<V>)`

```cpp
template<class V = R> inline Mock mockReturnValueOnce(std::type_identity_t< V > v) const requires (!std::is_void_v<V>)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1525?:1525

Makes the mock return `v` (once). Several can be queued, they are used in order.

#### Returns
Itself

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `v` | `std::type_identity_t< V >` | Value to return |

---

#### mockClear

`const` `inline`

```cpp
inline Mock mockClear() const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1534?:1534

Forgets the recorded calls and results, keeps the implementations.

#### Returns
Itself

---

#### mockReset

`const` `inline`

```cpp
inline Mock mockReset() const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1544?:1544

Calls `mockClear()` but also goes back to the implementation give to [jest::fn](#fn) (or to none).

#### Returns
Itself

---

#### mockName

`const` `inline`

```cpp
inline Mock mockName(std::string name) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1557?:1557

Names the mock in failure messages (defaults to "jest.fn()").

#### Returns
Itself

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `std::string` | Name to give |



## Anything

```cpp
#include <jest-cpp.hpp>
```

```cpp
struct Anything
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L109?:109

Placeholder that matches any single argument in the mock matchers.

Use the ready made object [jest::anything](#anything) rather than creating one

**See also**: [anything](#anything)



## Regex

```cpp
#include <jest-cpp.hpp>
```

```cpp
struct Regex
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L125?:125

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L127?:127

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L134?:134

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1331?:1331



## EachBinder

```cpp
template<class Row>
class EachBinder
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1297?:1297

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`EachBinder`](#eachbinder-1) `inline` |  |
| `void` | [`operator()`](#operator-1) `const` `inline` |  |

---

#### EachBinder

`inline`

```cpp
inline EachBinder(std::vector< Row > r, bool fail)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1303?:1303

---

#### operator()

`const` `inline`

```cpp
template<class F> inline void operator()(const std::string & name, F && fn, int timeoutMs = -1) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1305?:1305

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1299?:1299

---

#### failing

```cpp
bool failing
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1300?:1300



## DescribeEachBinder

```cpp
template<class Row>
class DescribeEachBinder
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1314?:1314

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`DescribeEachBinder`](#describeeachbinder-1) `inline` |  |
| `void` | [`operator()`](#operator-2) `const` `inline` |  |

---

#### DescribeEachBinder

`inline`

```cpp
inline DescribeEachBinder(std::vector< Row > r, bool)
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1319?:1319

---

#### operator()

`const` `inline`

```cpp
template<class F> inline void operator()(const std::string & name, F && fn, int = -1) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1321?:1321

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::vector< Row >` | [`rows`](#rows-1)  |  |

---

#### rows

```cpp
std::vector< Row > rows
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1316?:1316



## ExpectationFailed

```cpp
#include <jest-cpp.hpp>
```

```cpp
struct ExpectationFailed
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L165?:165

> **Inherits:** `runtime_error`

Thrown to indicate that an expectation failed its conditions.



## Matchers

```cpp
template<class T>
struct Matchers
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L428?:428

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
| `void` | [`toHaveBeenCalled`](#tohavebeencalled) `const` `inline` | Checks that the mock was called at least once. |
| `void` | [`toHaveBeenCalledTimes`](#tohavebeencalledtimes) `const` `inline` | Checks that the mock was called exactly `n` times. |
| `void` | [`toHaveBeenCalledTimes`](#tohavebeencalledtimes-1) `const` `inline` |  |
| `void` | [`toHaveBeenCalledTimes`](#tohavebeencalledtimes-2) `const` `inline` |  |
| `void` | [`toHaveBeenCalledWith`](#tohavebeencalledwith) `const` `inline` | Checks that at least one call received has exactly these arguments. |
| `void` | [`toHaveBeenLastCalledWith`](#tohavebeenlastcalledwith) `const` `inline` | Checks that the last call received has exactly these arguments. |
| `void` | [`toHaveBeenNthCalledWith`](#tohavebeennthcalledwith) `const` `inline` | Checks that call number `n` received exactly these arguments. |
| `void` | [`toHaveBeenNthCalledWith`](#tohavebeennthcalledwith-1) `const` `inline` |  |
| `void` | [`toHaveBeenNthCalledWith`](#tohavebeennthcalledwith-2) `const` `inline` |  |
| `void` | [`toHaveReturned`](#tohavereturned) `const` `inline` | Checks that the mock returned at least once. |
| `void` | [`toHaveReturnedTimes`](#tohavereturnedtimes) `const` `inline` | Checks that the mock returned exactly `n` times. |
| `void` | [`toHaveReturnedTimes`](#tohavereturnedtimes-1) `const` `inline` |  |
| `void` | [`toHaveReturnedTimes`](#tohavereturnedtimes-2) `const` `inline` |  |
| `void` | [`toHaveReturnedWith`](#tohavereturnedwith) `const` `inline` | Checks that at least one call returned `value`. |
| `void` | [`toHaveLastReturnedWith`](#tohavelastreturnedwith) `const` `inline` | Checks that the last call returned `value`. |
| `void` | [`toHaveNthReturnedWith`](#tohaventhreturnedwith) `const` `inline` | Checks that call number `n` returned `value`. |
| `void` | [`toHaveNthReturnedWith`](#tohaventhreturnedwith-1) `const` `inline` |  |
| `void` | [`toHaveNthReturnedWith`](#tohaventhreturnedwith-2) `const` `inline` |  |

---

#### toBe

`const` `inline`

```cpp
template<class U> inline void toBe(const U & expected) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L455?:455

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L469?:469

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L477?:477

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L483?:483

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L491?:491

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L501?:501

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L507?:507

Alias for toBeUndefined.

**See also**: toBeUndefined

---

#### toBeGreaterThan

`const` `inline`

```cpp
template<class U> inline void toBeGreaterThan(const U & expected) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L516?:516

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L527?:527

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L538?:538

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L549?:549

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L559?:559

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L578?:578

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L596?:596

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L602?:602

---

#### toHaveLength

`const` `inline`

```cpp
inline void toHaveLength(int n) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L609?:609

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L620?:620

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L631?:631

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L642?:642

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L646?:646

---

#### toThrow

`const` `inline`

```cpp
inline void toThrow() const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L654?:654

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L669?:669

Checks whether a callable throws an exception whose message contains a substring.

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `substr` | `const std::string &` | Substring expected in the exception message. |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the callable does not throw a matching exception. |

---

#### toHaveBeenCalled

`const` `inline`

```cpp
inline void toHaveBeenCalled() const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L693?:693

Checks that the mock was called at least once.

```cpp
expect(notify).toHaveBeenCalled();
```

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the mock was never called |

---

#### toHaveBeenCalledTimes

`const` `inline`

```cpp
inline void toHaveBeenCalledTimes(std::size_t n) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L707?:707

Checks that the mock was called exactly `n` times.

```cpp
expect(charge).toHaveBeenCalledTimes(2);
```

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `n` | `std::size_t` | Expected number of calls |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If it was called a different number of times |

---

#### toHaveBeenCalledTimes

`const` `inline`

```cpp
inline void toHaveBeenCalledTimes(unsigned int n) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L711?:711

---

#### toHaveBeenCalledTimes

`const` `inline`

```cpp
inline void toHaveBeenCalledTimes(int n) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L714?:714

---

#### toHaveBeenCalledWith

`const` `inline`

```cpp
template<class... E> inline void toHaveBeenCalledWith(const E &... expected) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L737?:737

Checks that at least one call received has exactly these arguments.

Arguments are compared like toEqual(), and [jest::anything](#anything) matches any value. The number of expected arguments must match the mock's parameters; this is checked when you compile

```cpp
expect(charge).toHaveBeenCalledWith(49.99);
expect(log).toHaveBeenCalledWith(3, anything);
```

**See also**: toHaveBeenLastCalledWith, toHaveBeenNthCalledWith

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `expected` | `const E &...` | The expected arguments |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If not call matches |

---

#### toHaveBeenLastCalledWith

`const` `inline`

```cpp
template<class... E> inline void toHaveBeenLastCalledWith(const E &... expected) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L761?:761

Checks that the last call received has exactly these arguments.

Arguments are compared like toEqual(), and [jest::anything](#anything) matches any value

**See also**: toHaveBeenCalledWith, toHaveBeenNthCalledWith

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `expected` | `const E &...` | The expected arguments |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the mock was never called, or its last call doesn't match |

---

#### toHaveBeenNthCalledWith

`const` `inline`

```cpp
template<class... E> inline void toHaveBeenNthCalledWith(std::size_t n, const E &... expected) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L785?:785

Checks that call number `n` received exactly these arguments.

Calls are counted from 1

```cpp
expect(notify).toHaveBeenNthCalledWith(1, "started");
```

**See also**: toHaveBeenCalledWith, toHaveBeenLastCalledWith

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `n` | `std::size_t` | Number of the call, starting at 1 |
| `expected` | `const E &...` | The expected arguments |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If there is no such call, or it doesn't match |

---

#### toHaveBeenNthCalledWith

`const` `inline`

```cpp
template<class... E> inline void toHaveBeenNthCalledWith(unsigned int n, const E &... expected) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L792?:792

---

#### toHaveBeenNthCalledWith

`const` `inline`

```cpp
template<class... E> inline void toHaveBeenNthCalledWith(int n, const E &... expected) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L796?:796

---

#### toHaveReturned

`const` `inline`

```cpp
inline void toHaveReturned() const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L809?:809

Checks that the mock returned at least once.

"Returned" means the call finished without throwing

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If no call returned |

---

#### toHaveReturnedTimes

`const` `inline`

```cpp
inline void toHaveReturnedTimes(std::size_t n) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L826?:826

Checks that the mock returned exactly `n` times.

"Returned" means the call finished without throwing

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `n` | `std::size_t` | Expected number of returns |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the count differs |

---

#### toHaveReturnedTimes

`const` `inline`

```cpp
inline void toHaveReturnedTimes(unsigned int n) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L835?:835

---

#### toHaveReturnedTimes

`const` `inline`

```cpp
inline void toHaveReturnedTimes(int n) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L838?:838

---

#### toHaveReturnedWith

`const` `inline`

```cpp
template<class V> inline void toHaveReturnedWith(const V & value) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L862?:862

Checks that at least one call returned `value`.

The value is compared like toEqual(). The mock must not return `void`

"Returned" means the call finished without throwing

```cpp
expect(charge).toHaveReturnedWith(true);
```

**See also**: toHaveLastReturnedWith, toHaveNthReturnedWith

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `const V &` | The expected return value |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If no call returned that value |

---

#### toHaveLastReturnedWith

`const` `inline`

```cpp
template<class V> inline void toHaveLastReturnedWith(const V & value) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L886?:886

Checks that the last call returned `value`.

The value is compared like toEqual(). A last call that threw doesn't match. The mock must not return `void`

"Returned" means the call finished without throwing

**See also**: toHaveReturnedWith, toHaveNthReturnedWith

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `const V &` | The expected return value |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If the last call didn't return that value |

---

#### toHaveNthReturnedWith

`const` `inline`

```cpp
template<class V> inline void toHaveNthReturnedWith(std::size_t n, const V & value) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L909?:909

Checks that call number `n` returned `value`.

Calls are counted from 1. The value is compared like toEqual(). The mock must not return `void`

"Returned" means the call finished without throwing

**See also**: toHaveLastReturnedWith, toHaveReturnedWith

##### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `n` | `std::size_t` | Number of the call, starting at 1 |
| `value` | `const V &` | The expected return value |

##### Exceptions

| Exception | Description |
|-----------|-------------|
| `detail::ExpectationFailed` | If there is no such call, or it returned something else |

---

#### toHaveNthReturnedWith

`const` `inline`

```cpp
template<class V> inline void toHaveNthReturnedWith(unsigned int n, const V & value) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L917?:917

---

#### toHaveNthReturnedWith

`const` `inline`

```cpp
template<class V> inline void toHaveNthReturnedWith(int n, const V & value) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L921?:921



## EachFn

```cpp
template<template< class > class Binder>
struct EachFn
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1334?:1334

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`failing`](#failing-1)  |  |

---

#### failing

```cpp
bool failing
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1335?:1335

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `auto` | [`operator()`](#operator-3) `const` `inline` `requires Range<R>` |  |
| `auto` | [`operator()`](#operator-4) `const` `inline` |  |

---

#### operator()

`const` `inline` `requires Range<R>`

```cpp
template<class R> inline auto operator()(const R & rows) const requires Range<R>
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1337?:1337

---

#### operator()

`const` `inline`

```cpp
template<class T> inline auto operator()(std::initializer_list< T > rows) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1342?:1342



## FailingApi

```cpp
struct FailingApi
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1348?:1348

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `EachFn< EachBinder >` | [`each`](#each)  |  |

---

#### each

```cpp
EachFn< EachBinder > each {true}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1349?:1349

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1350?:1350



## TestApi

```cpp
struct TestApi
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1355?:1355

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

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1356?:1356

---

#### failing

```cpp
FailingApi failing {}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1357?:1357

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`operator()`](#operator-6) `const` `inline` |  |

---

#### operator()

`const` `inline`

```cpp
inline void operator()(const std::string & name, const std::function< void()> & fn, int timeoutMs = -1) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1358?:1358



## DescribeApi

```cpp
struct DescribeApi
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1363?:1363

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `EachFn< DescribeEachBinder >` | [`each`](#each-2)  |  |

---

#### each

```cpp
EachFn< DescribeEachBinder > each {false}
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1364?:1364

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`operator()`](#operator-7) `const` `inline` |  |

---

#### operator()

`const` `inline`

```cpp
inline void operator()(const std::string & name, const std::function< void()> & fn) const
```

Defined in https://github.com/blorbee1/jest-cpp/blob/master/include/jest-cpp.hpp#L1365?:1365

Generated by [Moxygen](https://0state.com/moxygen)