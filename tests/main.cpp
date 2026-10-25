#include "jest-cpp.hpp"
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

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

int main(int argc, char** argv) {
    describe("math") {
        static int counter = 0;
        beforeEach { counter = 10; };

        it("adds numbers") {
            expect(add(2, 3)).toBe(5);
            expect(add(2, 3)).never.toBe(6);
            expect(add(2, 3)).toBeGreaterThan(4);
        };

        it("sees the hook", 1000) {
            expect(counter).toBe(10);
        };

        describe("nested") {
            it("fails on purpose") {
                expect(add(1, 1)).toBe(3);
            };
        }
    }

    describe("each") {
        it.each(table<int, int, int>({{1, 1, 2}, {1, 2, 3}, {2, 1, 3}}))("%d + %d = %d", [](int a, int b, int sum) {
            expect(add(a, b)).toBe(sum);
        });

        test.each({1, 2, 3})("%i is positive (#%#)", [](int n) {
            expect(n).toBeGreaterThan(0);
        });

        it.each(std::vector<std::pair<std::string, int>>{{"a", 1}, {"bb", 2}})("length of %s is %d", [](const std::string& s, int len) {
            expect(s).toHaveLength(static_cast<size_t>(len));
        });
    }

    describe.each({"alpha", "beta"})("suite for %s", [](const std::string& name) {
        it("has a name") {
            expect(name).toMatch("a");
        };
    });

    describe("failing") {
        test.failing("known bug", [] {
            expect(add(2, 2)).toBe(5);
        });

        it.failing.each({1, 2})("known bug %d", [](int n) {
            expect(n).toBe(0);
        });
    }

    describe("matchers") {
        it("toBe is shallow, toEqual is deep") {
            auto a = std::make_shared<int>(7), b = std::make_shared<int>(7);
            expect(a).never.toBe(b); // different addresses
            expect(a).toEqual(b); // same pointee

            expect(std::vector{1, 2, 3}).toEqual(std::vector{1, 2, 3}); // checks values
            expect((std::map<std::string, int>{{"x", 1}})).toEqual((std::map<std::string, int>{{"x", 1}}));
        };

        it("numbers follow Object.is") {
            expect(std::nan("")).toBe(std::nan("")); // NaN is NaN
            expect(0.0).never.toBe(-0.0); // +0 and -0 differ
            expect(0.1 + 0.2).toBeCloseTo(0.3);
        };

        it("optional/pointers") {
            std::vector<int> v{4, 5, 6};
            expect(find(v, 5)).toBeDefined();
            expect(find(v, 5)).toBe(1); // compared like optional == value
            expect(find(v, 9)).toBeUndefined();
            expect(find(v, 9)).toBeNull(); // alias to toBeUndefined()
            expect(find(v, 9)).toEqual(std::nullopt);
            expect(std::unique_ptr<int>()).toBeNull();
        };

        it("toMatch") {
            expect("hello world").toMatch("lo wo"); // substr
            expect("hello world").toMatch("w.r"_re); // regex (ECMAScript)
            expect("hello world").toMatch(std::regex("^hello"));
            expect("hello world").never.toMatch("bye"_re);
        };

        it("toThrow") {
            expect([] {
                throw std::runtime_error("boom");
            }).toThrow("boom");
        };
    }

    return run(argc, argv);
}