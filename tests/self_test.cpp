#include <cmath>
#include <map>
#include <memory>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "jest-cpp.hpp"

using namespace jest;

int add(int a, int b) {
    return a + b;
}

std::optional<int> find(const std::vector<int>& v, int x) {
    for (std::size_t i = 0; i < v.size(); ++i)
        if (v[i] == x)
            return static_cast<int>(i);
    return std::nullopt;
}

struct Rational {
    int n, d;
    Rational(int n_, int d_) : n(n_), d(d_) {}
    Rational operator+(const Rational& o) const { return {n * o.d + o.n * d, d * o.d}; }
};

int main(int argc, char** argv) {
    init(argc, argv);

    // core test execution
    describe("core") {
        it("runs a test successfully") {
            expect(true).toBe(true);
        };

        it("evaluates expressions") {
            expect(add(2, 3)).toBe(5);
            expect(-2 + 2).toBe(0);
            expect(add(0, 0)).toBe(0);
        };
    }

    // describe scopes and captured variables
    describe("fixtures") {
        Rational a(1, 2);
        Rational b(3, 5);

        int counter = 0;
        beforeEach { counter += 1; };

        it("preserved variables declared in describe") {
            expect(a.n).toBe(1);
            expect(a.d).toBe(2);
            expect(b.n).toBe(3);
            expect(b.d).toBe(5);
        };

        it("shared state with hooks") {
            expect(counter).toBe(2);
        };

        it("supports user defined operators") {
            Rational result = a + b;
            expect(result.n).toBe(11);
            expect(result.d).toBe(10);
        };
    }

    // hooks
    describe("hooks") {
        static int counter = 0;
        beforeEach { counter = 10; };

        it("runs beforeEach before the test") {
            expect(counter).toBe(10);
        };

        it("runs hooks for every test") {
            expect(counter).toBe(10);
        };
    }

    // nested describe blocks
    describe("nested suites") {
        int value = 42;
        describe("inner suite") {
            it("can access enclosing variables") {
                expect(value).toBe(42);
            };

            it("runs normal assertions") {
                expect(value).toBeGreaterThan(0);
                expect(value).toBeLessThan(100);
            };
        }
    }

    // parameterized tests
    describe("parameterized tests") {
        it.each(table<int, int, int>({{1, 1, 2}, {1, 2, 3}, {2, 1, 3}, {-1, 1, 0}}))("%d + %d = %d", [](int a, int b, int sum) {
            expect(add(a, b)).toBe(sum);
        });

        test.each({1, 2, 3, 4, 5})("%i is positive (#%#)", [](int n) {
            expect(n).toBeGreaterThan(0);
        });

        it.each(std::vector<std::pair<std::string, int>>{{"a", 1}, {"bb", 2}, {"hello", 5}})("length of %s is %d", [](const std::string& s, int length) {
            expect(s).toHaveLength(static_cast<size_t>(length));
        });
    }

    // parameterized describe blocks
    describe.each({"alpha", "beta"})("suite for %s", [](const std::string& name) {
        it("has a name") {
            expect(name).toMatch("a");
        };
    });

    describe.each({'a', 'b'})("suite for %s", [](const std::string& name) {
        it("matches its expected character") {
            expect(name).toMatch("a|b"_re);
        };
    });

    // basic matchers
    describe("basic matchers") {
        it("supprots toBe") {
            expect(5).toBe(5);
            expect("hello").toBe("hello");
        };

        it("supports negation") {
            expect(5).never.toBe(6);
            expect("hello").never.toBe("world");
        };

        it("supports numeric comparisons") {
            expect(5).toBeGreaterThan(4);
            expect(5).toBeGreaterThanOrEqual(5);
            expect(4).toBeLessThan(5);
            expect(5).toBeLessThanOrEqual(5);
        };

        it("supports approximate floating point equality") {
            expect(0.1 + 0.2).toBeCloseTo(0.3);
        };

        it("supports NaN equality") {
            expect(std::nan("")).toBe(std::nan(""));
        };

        it("distinguishes positive and negative zero") {
            expect(0.0).never.toBe(-0.0);
        };
    }

    // deep equality and identity
    describe("equality matchers") {
        it("distinguishes identity from deep equality") {
            auto a = std::make_shared<int>(7);
            auto b = std::make_shared<int>(7);

            expect(a).never.toBe(b);
            expect(a).toEqual(b);
        };

        it("compares vectors by value") {
            expect(std::vector{1, 2, 3}).toEqual(std::vector{1, 2, 3});
        };

        it("compares strings by value") {
            expect(std::string("hello")).toBe("hello");
            expect(std::string("hello")).toEqual("hello");
            expect(std::string("hello")).toBe(std::string("hello"));
        };

        it("compares maps by value") {
            expect(std::map<std::string, int>{{"x", 1}, {"y", 2}}).toEqual(std::map<std::string, int>{{"x", 1}, {"y", 2}});
        };
    }

    // optional values
    describe("optional and pointer matchers") {
        it("recognizes present optional values") {
            std::vector<int> values{4, 5, 6};

            expect(find(values, 5)).toBeDefined();
            expect(find(values, 5)).toBe(1);
        };

        it("recognizes missing optional values") {
            std::vector<int> values{4, 5, 6};

            expect(find(values, 9)).toBeUndefined();
            expect(find(values, 9)).toBeNull();
            expect(find(values, 9)).toEqual(std::nullopt);
        };

        it("recognizes null pointers") {
            expect(std::unique_ptr<int>()).toBeNull();
        };
    }

    // string and regex matchers
    describe("string matchers") {
        it("matchers substrings") {
            expect("hello world").toMatch("lo wo");
        };

        it("matches regular expressions from literals") {
            expect("hello world").toMatch("w.rld"_re);
        };

        it("matches std::regex") {
            expect("hello world").toMatch(std::regex("^hello"));
        };

        it("supports negated matching") {
            expect("hello world").never.toMatch("bye");
            expect("hello world").never.toMatch("bye"_re);
        };

        it("checks string length") {
            expect("hello").toHaveLength(5);
            expect(std::string("hello")).toHaveLength(5);
        };
    }

    // exception matchers
    describe("exception matchers") {
        it("matches thrown exception messages") {
            expect([] {
                throw std::runtime_error("boom");
            }).toThrow("boom");
        };

        it("matches exceptions from std") {
            expect([] {
                throw std::invalid_argument("invalid input");
            }).toThrow("invalid input");
        };
    }

    return run();
}