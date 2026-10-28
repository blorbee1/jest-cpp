#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include <jest-cpp.hpp>

using namespace jest;

// cant be default constructed
struct NoDefault {
    NoDefault() = delete;
    explicit NoDefault(int v) : value(v) {}
    int value;
};

int triple(int x) {
    return x * 3;
}

struct Order {
    double total;
};

bool checkout(const Order& order, const std::function<bool(double)>& charge, const std::function<void(const std::string&)>& notify) {
    if (!charge(order.total)) {
        notify("payment failed");
        return false;
    }
    notify("thanks");
    return true;
}

int main(int argc, char** argv) {
    init(argc, argv);

    describe("jest::fn recording") {
        it("records the arguments of every call") {
            auto m= fn<int(int, const std::string&)>();
            m(1, "a");
            m(2, std::string("b"));

            expect(m.mock.calls).toHaveLength(2);
            expect(std::get<0>(m.mock.calls[1])).toBe(2);
            expect(std::get<1>(m.mock.calls[0])).toBe("a");
            expect(m.mock.calls[1]).toEqual(std::make_tuple(2, std::string("b")));
        };

        it("records copies so later changes don't affect them") {
            std::vector<int> v{1, 2};
            auto m = fn<void(std::vector<int>)>();
            m(v);

            v.push_back(3);
            expect(std::get<0>(m.mock.calls[0])).toEqual(std::vector{1, 2});
        };

        it("records C strings as strings (the pointer may be gone)") {
            auto m = fn<void(const char*)>();
            m(std::string("abc").c_str()); // the temporary string is destroyed here

            expect(std::get<0>(m.mock.calls[0])).toBe("abc");
            expect(m).toHaveBeenCalledWith("abc");
        };

        it("records arguments before the implementation can change them") {
            auto m = fn<void(int&)>([](int& x) {
                x = 42;
            });
            int v = 0;
            m(v);

            expect(v).toBe(42);
            expect(std::get<0>(m.mock.calls[0])).toBe(0);
        };

        it("supports move only arguments (recorded as a placeholder)") {
            int seen = 0;
            auto m = fn<void(std::unique_ptr<int>)>([&](std::unique_ptr<int> p) {
                seen = *p;
            });

            m(std::make_unique<int>(7));
            expect(seen).toBe(7);
            expect(m).toHaveBeenCalledTimes(1);
            expect(m).toHaveBeenCalledWith(anything);
        };

        it("supports calls with no arguments") {
            auto m = fn<void()>();
            m();

            expect(m).toHaveBeenCalledWith();
            expect(m.mock.calls).toHaveLength(1);
        };
    }

    describe("jest::fn return values") {
        it("returns a default value when nothing is configured") {
            expect(fn<int()>()()).toBe(0);
            expect(fn<std::string()>()()).toBe("");
            expect(fn<std::optional<int>()>()()).toBeUndefined();

            auto v = fn<void()>();
            v();
            expect(v).toHaveBeenCalledTimes(1);
        };

        it("throws an error when the return type has no default") {
            auto m = fn<NoDefault()>();
            expect([&] {
                m();
            }).toThrow("no return value");
            expect(m).toHaveBeenCalledTimes(1);
        };

        it("mockReturnValue") {
            auto m = fn<int()>().mockReturnValue(5);
            expect(m()).toBe(5);
            expect(m()).toBe(5);
            m.mockReturnValue(6);
            expect(m()).toBe(6);
        };

        it("mockReturnValueOnce is used in order, then the normal value") {
            auto m = fn<int()>().mockReturnValueOnce(1).mockReturnValueOnce(2).mockReturnValue(9);
            expect(m()).toBe(1);
            expect(m()).toBe(2);
            expect(m()).toBe(9);
            expect(m()).toBe(9);
        };

        it("converts the value to the return type") {
            expect(fn<double()>().mockReturnValue(2)()).toBe(2.0);
            expect(fn<std::string()>().mockReturnValue("hi")()).toBe("hi");
        };

        it("mockImplementation and mockImplementationOnce") {
            auto m = fn<int(int)>()
                .mockImplementation([](int x) { return x + 1; })
                .mockImplementationOnce([](int x) { return x * 100; });
            expect(m(2)).toBe(200);
            expect(m(2)).toBe(3);
        };

        it("fn(impl) takes the signature from a lambda") {
            auto m = fn([](int a, int b) { return a + b; });
            expect(m(2, 3)).toBe(5);
            expect(m).toHaveBeenCalledWith(2, 3);
        };

        it("fn<Sig>(impl) converts the implementation to the signature") {
            auto m = fn<double(int)>([](int x) { return x / 2.0; });
            expect(m(3)).toBe(1.5);
        };

        it("fn(function pointer)") {
            auto m = fn(&triple);
            expect(m(4)).toBe(12);
            expect(m).toHaveBeenCalledWith(4);
        };
    }

    describe("jest::fn copies and conversions") {
        it("copies share one record") {
            auto a = fn<void(int)>();
            auto b = a;
            b(1);
            a(2);
            expect(a.mock.calls).toHaveLength(2);
            expect(b.mock.calls).toHaveLength(2);
        };

        it("works inside a std::function") {
            auto m = fn<int(int)>().mockReturnValue(7);
            std::function<int(int)> f = m;
            expect(f(1)).toBe(7);
            expect(m).toHaveBeenCalledWith(1);
        };

        it("can be passed where a std::function is expected") {
            auto m = fn<void(int)>();
            auto twice = [](const std::function<void(int)>& cb) { cb(1); cb(2); };
            twice(m);
            expect(m).toHaveBeenCalledTimes(2);
        };

        it("can be passed to a template") {
            auto m = fn<void(int)>();
            auto once = [](auto&& cb) { cb(5); };
            once(m);
            expect(m).toHaveBeenCalledWith(5);
        };

        it("can be called through a const handle") {
            const auto m = fn<void()>();
            m();
            expect(m).toHaveBeenCalledTimes(1);
        };
    }

    describe("jest::fn clearing") {
        it("mockClear forgets the calls but keeps the implementation") {
            auto m = fn<int()>().mockReturnValue(5);
            m();
            m.mockClear();
            expect(m.mock.calls).toHaveLength(0);
            expect(m.mock.results).toHaveLength(0);
            expect(m()).toBe(5);
        };

        it("mockReset goes back to the initial implementation") {
            auto m = fn([](int x) { return x; });
            m.mockReturnValue(5);
            expect(m(1)).toBe(5);
            m.mockReset();
            expect(m.mock.calls).toHaveLength(0);
            expect(m(1)).toBe(1);
        };

        it("mockReset with no initial implementation goes back to the default and drops queued values") {
            auto m = fn<int()>().mockReturnValue(3).mockReturnValueOnce(4);
            m.mockReset();
            expect(m()).toBe(0);
        };
    }

    describe("jest::fn exceptions and reentrancy") {
        it("records a throwing call and rethrows") {
            auto m = fn<int(int)>([](int x) -> int {
                if (x < 0) throw std::invalid_argument("negative");
                return x;
            });
            expect([&] { m(-1); }).toThrow("negative");
            expect(m(2)).toBe(2);
            expect(m.mock.calls).toHaveLength(2);
            expect(m.mock.results[0].type == ResultType::THROW).toBe(true);
            expect(m.mock.results[1].type == ResultType::RETURN).toBe(true);
            expect(m).toHaveReturnedTimes(1);
        };

        it("a mock can call itself") {
            auto m = fn<int(int)>();
            m.mockImplementation([&m](int n) -> int {
                return n <= 1 ? 1 : n * m(n - 1);
            });
            expect(m(4)).toBe(24);
            expect(m).toHaveBeenCalledTimes(4);
            expect(m).toHaveNthReturnedWith(1, 24);
            expect(m).toHaveNthReturnedWith(4, 1);
        };

        it("a call that hasnt finished is incomplete") {
            auto m = fn<int(int)>();
            bool outerWasIncomplete = false;
            m.mockImplementation([&](int n) -> int {
                if (n == 1) {
                    outerWasIncomplete = m.mock.results[0].type == ResultType::INCOMPLETE;
                    return 1;
                }
                return n * m(n - 1);
            });
            m(3);
            expect(outerWasIncomplete).toBe(true);
        };
    }

    describe("mock matchers") {
        it("toHaveBeenCalled") {
            auto m = fn<void(int)>();
            expect(m).never.toHaveBeenCalled();
            m(1);
            expect(m).toHaveBeenCalled();
        };

        it("toHaveBeenCalledTimes") {
            auto m = fn<void(int)>();
            m(1);
            m(2);
            expect(m).toHaveBeenCalledTimes(2);
            expect(m).never.toHaveBeenCalledTimes(1);
        };

        it("toHaveBeenCalledWith compares like toEqual, and any call can match") {
            auto m = fn<void(int, std::string, std::vector<int>)>();
            m(1, "a", {1, 2});
            m(2, "b", {});
            expect(m).toHaveBeenCalledWith(1, "a", std::vector<int>{1, 2});
            expect(m).toHaveBeenCalledWith(2, "b", std::vector<int>{});
            expect(m).never.toHaveBeenCalledWith(2, "a", std::vector<int>{1, 2});
            expect(m).never.toHaveBeenCalledWith(1, "b", std::vector<int>{1, 2});
        };

        it("toHaveBeenCalledWith compares pointers by what they point to") {
            auto m = fn<void(const int*)>();
            int a = 5, b = 5, c = 6;
            m(&a);
            expect(m).toHaveBeenCalledWith(&b);
            expect(m).never.toHaveBeenCalledWith(&c);
        };

        it("jest::anything matches any single argument") {
            auto m = fn<void(int, std::string)>();
            m(1, "x");
            expect(m).toHaveBeenCalledWith(1, anything);
            expect(m).toHaveBeenCalledWith(anything, "x");
            expect(m).toHaveBeenCalledWith(anything, anything);
            expect(m).never.toHaveBeenCalledWith(2, anything);
        };

        it("toHaveBeenLastCalledWith and toHaveBeenNthCalledWith") {
            auto m = fn<void(int)>();
            expect(m).never.toHaveBeenLastCalledWith(1);
            m(1);
            m(2);
            m(3);
            expect(m).toHaveBeenLastCalledWith(3);
            expect(m).never.toHaveBeenLastCalledWith(2);
            expect(m).toHaveBeenNthCalledWith(1, 1);
            expect(m).toHaveBeenNthCalledWith(2, 2);
            expect(m).never.toHaveBeenNthCalledWith(2, 3);
            expect(m).never.toHaveBeenNthCalledWith(0, 1);
            expect(m).never.toHaveBeenNthCalledWith(4, 3);
        };

        it("toHaveReturned, toHaveReturnedTimes") {
            auto m = fn<int(int)>([](int x) { return x * 2; });
            expect(m).never.toHaveReturned();
            m(1);
            m(2);
            expect(m).toHaveReturned();
            expect(m).toHaveReturnedTimes(2);
            expect(m).never.toHaveReturnedTimes(3);
            auto v = fn<void()>();
            v();
            expect(v).toHaveReturned();
        };

        it("toHaveReturnedWith, toHaveLastReturnedWith, toHaveNthReturnedWith") {
            auto m = fn<int(int)>([](int x) { return x * 2; });
            m(1);
            m(5);
            expect(m).toHaveReturnedWith(2);
            expect(m).toHaveReturnedWith(10);
            expect(m).never.toHaveReturnedWith(3);
            expect(m).toHaveLastReturnedWith(10);
            expect(m).never.toHaveLastReturnedWith(2);
            expect(m).toHaveNthReturnedWith(1, 2);
            expect(m).never.toHaveNthReturnedWith(1, 10);
            expect(m).never.toHaveNthReturnedWith(3, 10);
        };
    }

    describe("mock matcher failure messages") {
        it("say what was expected and what was received") {
            auto m = fn<int(int, const std::string&)>().mockName("lookup");
            m(1, "a");
            m(2, "b");

            expect([&] {
                expect(m).toHaveBeenCalledTimes(3);
            }).toThrow("toHaveBeenCalledTimes(3)");

            expect([&] {
                expect(m).toHaveBeenCalledTimes(3);
            }).toThrow("lookup was called 2 times");

            expect([&] {
                expect(m).toHaveBeenCalledWith(3, "c");
            }).toThrow(R"(toHaveBeenCalledWith(3, "c"))");

            expect([&] {
                expect(m).toHaveBeenCalledWith(3, "c");
            }).toThrow(R"(2: (2, "b"))");

            expect([&] {
                expect(m).never.toHaveBeenCalled();
            }).toThrow("never.toHaveBeenCalled()");

            expect([&] {
                expect(m).toHaveBeenNthCalledWith(1, 9, anything);
            }).toThrow("toHaveBeenNthCalledWith(1, 9, anything)");
        };

        it("describes the results") {
            auto m = fn<int(int)>([](int x) -> int {
                if (x < 0) throw std::runtime_error("negative");
                return x;
            }).mockName("check");
            m(3);

            expect([&] {
                m(-1);
            }).toThrow();

            expect([&] {
                expect(m).toHaveReturnedWith(7);
            }).toThrow("toHaveReturnedWith(7)");

            expect([&] {
                expect(m).toHaveReturnedWith(7);
            }).toThrow("1: returned 3");

            expect([&] {
                expect(m).toHaveReturnedWith(7);
            }).toThrow("2: threw");
        };

        it("say when a mock was never called, using its name") {
            auto m = fn<void()>().mockName("charge");
            expect([&] {
                expect(m).toHaveBeenCalled();
            }).toThrow("charge was not called");
            expect(fn<void()>().getMockName()).toBe("jest.fn()");
        };
    }

    // a realistic use test case
    describe("checkout (dependency injection)") {
        auto charge = fn<bool(double)>().mockName("charge");
        auto notify = fn<void(const std::string&)>().mockName("notify");

        beforeEach {
            charge.mockReset();
            notify.mockReset();
        };

        it("charges the total and says thanks") {
            charge.mockReturnValue(true);
            expect(checkout(Order{49.99}, charge, notify)).toBe(true);
            expect(charge).toHaveBeenCalledTimes(1);
            expect(charge).toHaveBeenCalledWith(49.99);
            expect(notify).toHaveBeenCalledWith("thanks");
        };

        it("reports a failed payment") {
            charge.mockReturnValue(false);
            expect(checkout(Order{10.0}, charge, notify)).toBe(false);
            expect(notify).toHaveBeenLastCalledWith("payment failed");
            expect(notify).never.toHaveBeenCalledWith("thanks");
        };

        it("starts every test with a clean mock") {
            expect(charge).never.toHaveBeenCalled();
            expect(notify).never.toHaveBeenCalled();
        };
    }

    return run();
}