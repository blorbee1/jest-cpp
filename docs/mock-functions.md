# Mock functions

A mock is a stand in for something your code depends on. It lets you test one piece of code on its own, and check how it talked to that dependency.

Say you're testing `checkout()`, which charges a card and sends a notification. You don't want your tests to charge real cards or to fail when the network does.
So you pass in fake functions instead. A mock is a fake that also remembers every call, so afterward you can ask; was it called, how many times, with what?

```cpp
auto charge fn<bool(double)>().mockReturnValue(true);  // a fake that always succeeds
auto notify fn<void(const std::string&)>();

checkout(order, charge, notify);

expect(charge).toHaveBeenCalledTimes(1);
expect(charge).toHaveBeenCalledWith(49.99);
expect(notify).toHaveBeenCalledWith("thanks");
```

Mocks are useful for:
- Isolating the code under test from slow or unreliable things (networks, databases, clocks, files)
- Simulating cases that are hard to trigger for real, such as payment is declined, the server times out
- Verifying interactions, when the behavior you care about is that something was called (an email sent, a callback fired)
- Testing callbacks

A complete runnable program is in [`examples/mocks.cpp`](https://github.com/blorbee/jest-cpp/blob/main/examples/mocks.cpp) and on the [examples page](examples.mdx).

## Creating a mock

`jest::fn` creates a mock. Give it the function signature or give it an implementation to take the signature from:

```cpp
auto m = fn<int(int, int)>(); // no implementation, returns default value of return value, in this case 0
auto add = fn([](int a, int b) { return a + b; }); // the signature comes from the lambda
auto half = fn<double(int)>([](int x) { return x / 2.0; });
auto tri = fn(&triple); // a function pointer works too
```

A mock is called like any function: `add(2, 3)`

## Giving a mock to your code

A mock converts to a `std::function` with the same signature and works anywhere a callable is expected, includign template parameters:

```cpp
auto attempt = fn<bool()>().mockReturnValueOnce(false).mockReturnValue(true);
retry(attempt, 3); // takes a const std::function<bool()>&
expect(attempt).toHaveBeenCalledTimes(2);

auto cb = fn<void(int)>();
twice(cb); // a template: twice(Callback&& cb)
expect(cb).toHaveBeenCalledWith(2);
```

A mock is a cheap handle onto a shared satte. Copying it (which is what happens when it goes into a `std::function`) keeps recording into the same place, so your original variable sees every call.

## What a mock returns

When a mock is called, it does the first of these that applies:

1. runs the next **once** implementation or value that was queued
2. runs its **implementation** (the one given to `fn`, or set later)
3. returns a default value: `R{}`, so `0`, `""`, an empty `std::optional`... If the return type can't be default constructed, it throws a `std::logic_error` explaining that. A `void` mock does nothing.

| Method | Effect |
|---|---|
| `mockReturnValue(v)` | return `v` from now on |
| `mockReturnValueOnce(v)` | return `v` for the next call only |
| `mockImplementation(f)` | run `f` from now on |
| `mockImplementationOnce(f)` | run `f` for the next call only |

The `Once` variants queue up and are used in order. Every method returns the mock, so they chain:

```cpp
auto m = fn<int()>().mockReturnValueOnce(1).mockReturnValueOnce(2).mockReturnValue(9);
m(); // 1
m(); // 2
m(); // 9 
m(); // 9 
```

An implementation can do anything, including throwing:

```cpp
auto parse fn<int(const std::string&)>();
parse.mockImplementationOnce([](const std::string&) -> int {
    throw std::invalid_argument("bad input");
});
parge.mockReturnValue(7);
```

## Looking at what happened

`m.mock.calls` has one `std::tuple` of arguments per call, and `m.mock.results` has one entry per call saying what it did:

```cpp
auto m = fn<int(int, const std::string&)>().mockReturnValue(1);
m(1, "a");
m(2, "b");

expect(m.mock.calls).toHaveLength(2);
expect(std::get<0>(m.mock.calls[1])).toBe(2);       // first argument of the second call
expect(std::get<1>(m.mock.calls[1])).toBe("b");
expect(m.mock.results[0].type == ResultType::RETURN).toBe(true);
expect(m.mock.results[0].value).toEqual(1);         // an std::optional holding the returned value
```

| `results[i]` | |
|---|---|
| `type` | `ResultType::RETURN` (finished normally), `ResultType::THROW`, or `ResultType::INCOMPLETE` (the call is still running, for example while a mock calls itself) |
| `value` | the returned value, as an `std::optional` (empty for `void`) |
| `error` | the exception, as a `std::exception_ptr`, when the call threw |

Usually you don't dig into these directly. The matchers below do you it for you.

## Matchers

Pass a mock to `expect` to use the mock matchers. All the work with `.never`.

See [mock matchers](matchers.md#mock-matchers)

```cpp
auto charge = fn<bool(double)>().mockReturnValue(true);
charge(49.99);

expect(charge).toHaveBeenCalled();
expect(charge).toHaveBeenCalledTimes(1);
expect(charge).toHaveBeenCalledWith(49.99);
expect(charge).toHaveReturnedWith(true);
expect(charge).never.toHaveBeenCalledWith(5.0);
```

Expected arguments are compared like [`toEqual`](matchers.md#toequalexpected): strings by content (so `"abc"` matches a `std::string`),
containers element by element, and pointers by what they point to. The number of expected arguments must match the mock's parameters, and this is checked when you compile

`jest::anything` matches to any single argument:

```cpp
auto log = fn<void(int, const std::string&)>();
log(3, "disk full");

expect(log).toHaveBeenCalledWith(3, anything);
expect(log).toHaveBeenCalledWith(anything, "disk full");
```

Failure messages show every call the mock received, so you can see what happened instead:

```text
FAIL checkout > charges the total
    expect(received).toHaveBeenCalledWith(3, "c")
        Received: jest.fn() was called 2 times
            1: (1, "a")
            2: (2, "b")
```

## Clearing and resetting

| Method | Effect |
|---|---|
| `mockClear()` | forgets the recorded calls and results, and keeps the implementations |
| `mockReset()` | does `mockClear()`, and also goes back to the implementation given to `fn` (or to none), dropping queued once values |

Reset your mocks in `beforeEach` so tests don't affect each other:

```cpp
describe("checkout") {
    auto charge = fn<bool(double)>();
    beforeEach { charge.mockReset(); };
    
    it("...") { /* charge starts every test with no calls and no return value */ };
}
```

## Naming a mock

`mockName("name")` changes how the mock appears in failure messages from `jest.fn()` to the name:

```text
Received: charge was not called
```

## What gets recorded

- Arguments are recorded as copies, taken at call time. Changing a variable afterward doesn't change the record
- `const char*` arguments are recorded as `std::string`, because the pointer may be gone by the time you check
- References pass through. A mock with an implementation taking `int&` can change the caller's variable. The record has the value the argument had when the call started
- Move only arguments such as `std::unique_ptr` can't be copied, so they are recorded as a `<not recorded>` placeholder. The call still works, the implementation receives the real object, and you can still match such an argument with `anything`

## Exceptions

If an implementation throws, the mock records the call, marks its result as `THROW`, and rethrows the exception. `toHaveReturned` and `toHaveReturnedTimes` count only calls that finished without throwing.

## Making code mockable

Unlike JavaScript, C++ can't replace a function at runtime, so there is not `jest.spyOn`. Instead, code is written so its dependencies can be passed in. There are three ways:

1. `std::function`. Take the dependency as a `std::function` parameter or member. This is the simplest:

```cpp
class Checkout {
public:
    Checkout(std::function<bool(double)> charge, std::function<void(const std::string&)> notify);
};

Checkout checkout(charge, notify); // charge and notify are mocks
```

2. A template parameter. Takes any callable

```cpp
template <class Charge>
bool pay(Charge&& charge, double total) {
    return charge(total);
}
```

3. A virtual interface. For classes with several methods, write a small fake that implements the interface and forwards each method to a mock:

```cpp
struct PaymentGateway {
    virtual ~PaymentGateway() = default;
    virtual bool charge(double amount) = 0;
};

struct FakeGateway : PaymentGateway {
    Mock<bool(double)> chargeMock = fn<bool(double)>();
    bool charge(double amount) override { 
        return chargeMock(amount);
    }
};

FakeGateway gateway;
gateway.chargeMock.mockReturnValue(false);
receipt(gateway, 12.5); // the code under test takes a PaymentGateway&
expect(gateway.chargeMock).toHaveBeenCalledWith(12.5);
```

## Limitations

- Mocks are **not thread safe**
- A mock handle can't be assigned to another (copying is fine)
- A mock can't return a reference, return a value or a pointer
- Implementations are stored in a `std::function`, so they must be copyable
- Don't capture a mock by value inside tis own implementation. That creates a reference cycle and leaks, capture if by reference (`[&m]`)

## Differences from Jest

| Jest | jest-cpp |
|---|---|
| `jest.fn()` | `fn<Sig>()`, or `fn(lambda)` |
| `mock.calls[1][0]` | `std::get<0>(m.mock.calls[1])` |
| `mock.results[0].type === 'return'` | `m.mock.results[0].type == ResultType::RETURN` |
| `expect.anything()` | `jest::anything` |
| `mockFn.mockReturnValue(v)` and the other chainable methods | the same names |
| `mockClear`, `mockReset` | the same; `mockReset` restores the initial implementation |
| `mock.lastCall`, `mock.instances`, `mock.contexts` | not implemented (use `m.mock.calls.back()`) |
| `jest.spyOn`, `mockRestore` | not possible in C++; make dependencies injectable instead |
| `mockResolvedValue`, `mockRejectedValue`, `mockReturnThis` | not implemented |
| `expect.any(Type)`, other asymmetric matchers | not implemented |
| `toBeCalled` and the other deprecated aliases | not implemented; use the `toHaveBeen...` names |