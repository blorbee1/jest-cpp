# Failing tests and timeouts

## Known failures: `.failing`

`it.failing` (alias `test.failing`) is for a test that is expected to fail, such as a known bug you want to document:

```cpp
it.failing("known bug: 2 + 2", [] {
    expect(add(2, 2)).toBe(5);
});

it.failing.each({1, 2})("known bug %d", [](int n) {
    expect(n).toBe(0);
});
```

The test passes when its body fails. If the test manages to succeed, then the test will fail with:

```text
Failing test passed even though it was supposed to fail.
```

That is your cue to delete the `.failing` or figure out what is causing it to succeed

## Timeouts

Every test has a time limit, 5000 ms by default, change it for the whole run with [`--timeout <ms>`](command-line.md), or per test with:

```cpp
it("fast", 100) { ... }
it.each(rows)("name", fn, 100);
it("fast", [] { ... }, 100);
```

A test that takes too long is reported as failed:

```text
FAIL demo > is too slow
    Exceeded timeout of 10 ms for a test (took 50 ms).
```

**Note:** tests are synchronous, so the limit is checked **after** the test returns. A test that runs too long is marked failed, but an infinite loop will still hang.