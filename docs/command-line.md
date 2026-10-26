# Command line

Pass `main`'s arguments to `jest::init(argc, argv)` and your test program understands these options:

```text
-v, --verbose                  print every test result, grouped by describe block
-t, --testNamePattern <regex>  only run tests whose full name matches the regex
    --timeout <ms>             default per-test timeout (default 5000)
    --no-color                 disable colored output (also honors NO_COLOR)
-h, --help                     show this help
```

Long options also accept `--name=value`.

## Quiet and verbose output

By default only failures and the summary are printed. With `--verbose`, every test is listed under its `describe` block, with timings for tests that took at least a milisecond:

```text
Counter
    PASS starts at zero
    PASS counts
strings
    PASS can be searched
    nested describes
        PASS see the outer locals
        
Tests:  0 failed, 4 passed, 4 total
Time:   0.002 s
```

## Running some tests

`--testNamePattern` (or `-t`) is a regular expression searched in a test's **full name** (its `describe` names followed by its own name, separated by spaces).

```sh
./tests -t "Counter"          # everything inside describe("Counter")
./tests -t "Counter counts"   # one test
./tests -t "adds|subtracts"   # either
```

Tests that don't match are counted as skipped in the summary, and a `describe` block with no matching tests doesn't run its `beforeAll`/`afterAll` hooks. Code in the `describe` body itself still runs.

## Exit codes

| Code | Meaning |
|---|---|
| `0` | every test passed |
| `1` | at least one test failed |
| `2` | bad command line (unknown option, missing value, invalid regex, etc) |