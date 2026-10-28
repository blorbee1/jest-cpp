# Matcher reference

Every matcher is called on `expect(value)`. Put `.never` in front to negate it:

```cpp
expect(x).toBe(1);
expect(x).never.toBe(2);
```

A failed matcher throws, ending that test. The message shows the matcher call and the received value:

```text
FAIL math > nested > fails on purpose
    expect(received).toBe(3)
    Received: 2
```

## Equality

### `toBe(expected)`

Shallow equality, like `Object.is` from JavaScript.

- `NaN` equals `NaN`, `+0` is **not** equal to `-0`. Integers of different signs compare by value (`-1` never equals `4294967295u`)
- Pointers and smart pointers compare by address
- Everything else uses `operator==`, the test will error if the operator is not defined.

```cpp
expect(add(2, 3)).toBe(5);
expect(std::nan("")).toBe(std::nan(""));
expect(a_ptr).never.toBe(b_ptr);           // different objects
expect(std::optional<int>(5)).toBe(5);     // optional == value
```

### `toEqual(expected)`

Deep equality. Same number rules as `toBe`, but it looks through to the value:

- `std::optional` is equal when both are empty, or both hold equal values (an optional also equals a plain value and `std::nullopt`)
- Pointers and smart points compare by their values, so equal when both are nullptr or their pointees are deeply equal
- Containers (anything with `std::begin`/`std::end` defined) are equal when they have the same length and every element is deeply equal in the same order
- Tuples, pairs, and arrays equate by element
- Strings, including `const char*` and `std::string_view` equate by content

```cpp
expect(std::make_shared<int>(1)).toEqual(std::make_shared<int>(1));
expect(std::vector{1, 2, 3}).toEqual(std::vector{1, 2, 3});
expect(find(v, 9)).toEqual(std::nullopt); // find returns a std::optional
```

## Truthiness and emptiness

| Matcher | Passes when                                                                                                  |
|---|--------------------------------------------------------------------------------------------------------------|
| `toBeTruthy()` | the value converts to `true`; non-empty strings; `NaN` is falsy; a class without a bool conversion is truthy |
| `toBeFalsy()` | the opposite                                                                                                 |
| `toBeDefined()` | an `std::optional` has a value, a pointer is non-null; any other value is always defined                     |
| `toBeUndefined()` | an `std::optional` is empty or a pointer is null                                                             |
| `toBeNull()` | alias to `toBeUndefined()`                                                                                   |

## Numbers

| Matcher | Passes when |
|---|---|
| `toBeGreaterThan(x)` | `received > x` |
| `toBeGreaterThanOrEqual(x)` | `received >= x` |
| `toBeLessThan(x)` | `received < x` |
| `toBeLessThanOrEqual(x)` | `received <= x` |
| `toBeCloseTo(x, digits = 2)` | `abs(x - received) < 10^-digits / 2` |

## Strings and containers

### `toContain(item)`

- string: contains the substring (or character) `item`
- container: some element equals `item`

### `toHaveLength(n)`

The length of a string or the number of elements in a container

### `toMatch(pattern)`

The received value must be a string

| Argument | Meaning                                                       |
|---|---------------------------------------------------------------|
| `"text"` or `std::string` | the received string **contains** it                           |
| `"regex"_re` | a regular expression **search** (ECMAScript syntax)           |
| `std::regex` | same search, but the pattern is not shown in failure messages |

```cpp
expect("hello world").toMatch("lo wo");
expect("hello world").toMatch("^h.*d$"_re);
```

## Exceptions

`expect(callable)` where `callable` takes no arguments:

| Matcher | Passes when |
|---|---|
| `toThrow()` | calling it throws anything |
| `toThrow("text")` | it throws a `std::exception` whose `what()` contains `text` |

```cpp
expect([] {
    parse("not json");
}).toThrow("unexpected token");
```

## Mock matchers

`toHave...` matcher functions work only on mocks made with `jest::fn`

| Matcher | Passes when |
|---|---|
| `toHaveBeenCalled()` | the mock was called at least once |
| `toHaveBeenCalledTimes(n)` | it was called exactly `n` times |
| `toHaveBeenCalledWith(args...)` | **some** call had exactly these arguments |
| `toHaveBeenLastCalledWith(args...)` | the **last** call had these arguments |
| `toHaveBeenNthCalledWith(n, args...)` | call number `n` had these arguments (`n` starts at 1) |
| `toHaveReturned()` | at least one call returned (didn't throw) |
| `toHaveReturnedTimes(n)` | exactly `n` calls returned |
| `toHaveReturnedWith(value)` | some call returned `value` |
| `toHaveLastReturnedWith(value)` | the last call returned `value` |
| `toHaveNthReturnedWith(n, value)` | call number `n` returned `value` |