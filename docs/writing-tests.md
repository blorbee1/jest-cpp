# Writing tests

## describe and it

```cpp
describe("Stack") {
    it("starts empty") {
        expect(Stack{}.size()).toBe(0);
    };
    
    describe("when it has items") {
        it("reports the size") { ... };
    }
}
```

- `describe("name") { ... }` groups tests and can be nested to any depth.
- `it("name") { ... };` is one test, `test` is an alias.
- `it("name", 500) { ... };` sets a [timeout](failing-and-timeouts.md) of 500 ms for this test only.

## How tests run

Tests run **immediately, in source order**, at the point where they are written. `describe` simply runs its body and `it` runs its body right away inside a try/catch

This has some useful applications:

- Plain local variables in a `describe` block are alive for every test inside it
- Tests in the same `describe` can share (and change) those locals. If you need a clean state, reset in `beforeEach`
- Output appears as the tests run and is flushed after each result, so you still see progress if the code crashes.

## Fixtures

Declare shared state as ordinary locals:

```cpp
describe("Stack") {
    Stack s;    // lives until the closing brace of this describe
    beforeEach { s.clear(); };
    
    it("starts empty") {
        expect(s.size()).toBe(0);
    };
    it("pushes") {
        s.push(1);
        expect(s.size()).toBe(1);
    };
}
```

## Hooks

| Hook | Runs |
|---|---|
| `beforeAll { ... };` | once, right before the first test in the block that actually runs |
| `beforeEach { ... };` | before every test in the block, including nested blocks |
| `afterEach { ... };` | after every test, even when it failed |
| `afterAll { ... };` | once, when the `describe` block exits, if any of its tests ran |

- A hook applies only to tests declared **after** it
- `afterAll` runs after the block's locals are destroyed, so it must only use statics or variables from an outer scope
- If a hook throws, the failure is reported. For `beforeEach` and `afterEach` it counts as that test's failure.
- Hooks need the macros: they are not available with `JEST_NO_MACROS` unless you want to type the long expansion