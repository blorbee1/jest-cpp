# Table driven tests

`it.each(rows)(name, fn, timeout)`, `test.each(...)`, and `describe.each(...)` run the same body for each row of a table

`.each` can combine with [`.failing`](failing-and-timeouts.md) chaining them together into one statement. The optional third argument is a timeout in milliseconds

## Tuple rows

Rows that are tuples (or pairs/arrays) are spread into the callback's parameters. `table<...>()` spells out the column types of a braced table:

```cpp
it.each(table<int, int, int>({{1, 1, 2}, {2, 3, 5})))("%d + %d = %d", [](int a, int b, int sum) {
    expect(a + b).toBe(sum);
});
```

## Flat lists

A flat list is one value per row. String and character literals work too:

```cpp
it.each({1, 2, 3})("%i is positive), [](int n) {});

it.each({"a", "bb"})("%s is not empty", [](const std::string& s) {});
```

## Any container

```cpp
it.each(std::vector<std::pair<std::string, int>>{{"a", 1}, {"bb", 2}})("%s has length %d", [](confg std::string& s, int len) {});
```

## describe.each

`describe.each` creates one `describe` block per row. Inside it you write tests with the normal block syntax, and they can use the callback's parameters:

```cpp
describe.each({"alpha", "beta"})("suite for %s", [](const std::string& name) {
    it("has a name") {
        expect(name.size()).toBeGreaterThan(0);
    };
});
```

## Titles

The title is a template, placeholders are filled left to right from the row's values:

| Placeholder | Meaning |
|---|---|
| `%s` `%d` `%i` `%f` | the value as plain text |
| `%j` `%o` `%p` | the value pretty-printed (strings are quoted) |
| `%#` | zero-based row index |
| `%$` | one-based row number |
| `%%` | a literal `%` |

```text
PASS 1 + 1 = 2
PASS 1 is positive (row 0)
PASS bb is not empty
```