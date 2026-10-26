// matchers.cpp - a tour of the matchers

#include <cmath>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "jest-cpp.hpp"

using namespace jest;

std::optional<int> find(const std::vector<int>& v, int x) {
    for (std::size_t i = 0; i < v.size(); ++i)
        if (v[i] == x) return static_cast<int>(i);
    return std::nullopt;
}

int main(int argc, char** argv) {
    init(argc, argv);

    describe("equality") {
        it("toBe is shallow, toEqual is deep") {
            auto a = std::make_shared<int>(7);
            auto b = std::make_shared<int>(7);

            expect(a).never.toBe(b); // toBe when given with pointers will compare the MEMORY ADDRESSES they are pointing to
            expect(a).toEqual(b); // toEqual will go one step further and follow the points to compare their VALUES

            expect(std::vector{1, 2, 3}).toEqual(std::vector{1, 2, 3}); // toEqual will compare each entry of iterable objects
            expect(std::map<std::string, int>{{"x", 1}}).toEqual(std::map<std::string, int>{{"x", 1}});
        };

        it("numbers follow Object.is") {
            // a rough approximation of Object.is from JS
            expect(std::nan("")).toBe(std::nan("")); // NaN is NaN
            expect(0.0).never.toBe(-0.0); // +0 and -0 are different

            expect(0.1 + 0.2).toBeCloseTo(0.3); // this uses floating point approximation to see if they are equal
            expect(0.1 + 0.2).toBeCloseTo(0.3, 5); // the second argument is the amount of digits (after the decimal point) you want to check for
        };
    }

    describe("optionals and pointers") {
        it("maps onto defined, undefined, null") {
            std::vector<int> v{4, 5, 6};

            expect(find(v, 5)).toBeDefined(); // this passes if the std::optional has a value
            expect(find(v, 5)).toBe(1); // compares like optional == value

            expect(find(v, 9)).toBeUndefined(); // passes if the std::optional has no value
            expect(find(v, 9)).toBeNull(); // alias to toBeUndefined()
            expect(find(v, 9)).toEqual(std::nullopt); // alias to tobeUndefined()

            expect(std::unique_ptr<int>()).toBeNull(); // works with pointers as well
            expect(nullptr).toBeNull(); // raw pointers work too
        };
    }

    describe("numbers, strings, containers") {
        it("compares") {
            std::vector<int> v{1, 2, 3};

            expect(v.size()).toBeGreaterThan(2); // there are no sign compare warnings (ex size_t vs int)
            expect(v).toContain(2);
            expect(std::string("abc")).toContain("bc");

            expect("hello world").toMatch("lo wo"); // when given a simple string, it checks a substring
            expect("hello world").toMatch("^h.*d$"_re); // when given a regex, uses a regex search
        };
    }

    describe("exceptions") {
        it("toThrow") {
            expect([] {
                throw std::runtime_error("boom");
            }).toThrow();
            // simple .toThrow() will pass if the function ever throws

            expect([] {
                throw std::runtime_error("boom");
            }).toThrow("boom");
            // .toThrow() with an argument will only pass if the function throws with the same error message

            expect([] {}).never.toThrow(); // never can be used
        };
    }

    return run();
}