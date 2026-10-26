// each_and_failing.cpp - table driven tests and tests that are expected to fail

#include <string>

#include "jest-cpp.hpp"

using namespace jest;

int add(int a, int b) {
    return a + b;
}

int main(int argc, char** argv) {
    init(argc, argv);

    describe("each") {
        // rows that are tuples are spread into the callback's parameters
        // table<...>() spells out the column types of a braced table
        it.each(table<int, int, int>({{1, 1, 2}, {1, 2, 3}, {2, 1, 3}}))("%d + %d = %d", [](int a, int b, int sum) {
            expect(add(a, b)).toBe(sum);
        });
        // you can use printf-like string builders (%d, %i, %s, etc) to fill in the blanks of the test name
        // in this example, the output you will be seeing is
        // 1 + 1 = 2
        // 1 + 2 = 3
        // 2 + 1 = 3
        // TODO: finish explaination of this

        // using a flat list just loops over each entry
        it.each({1, 2, 3})("%i is positive (row %#)", [](int n) {
            expect(n).toBeGreaterThan(0);
        });

        // string literals work as rows too
        it.each({"a", "bb", "ccc"})("%s is not empty", [](const std::string& s) {
            expect(s.empty()).toBe(false);
        });
    }

    // describe.each registers one describe block per row
    describe.each({"alpha", "beta"})("suite for %s", [](const std::string& name) {
        it("has a name") {
            expect(name.size()).toBeGreaterThan(0);
        };
    });

    describe("failing") {
        // it.failing passes ONLY IF the body fails, handy for documenting known bugs
        it.failing("known bug: 2 + 2", [] {
            expect(add(2, 2)).toBe(5);
        });

        // you can chain .failing and .each to form more complicated tests
        it.failing.each({1, 2})("known bug %d", [](int n) {
            expect(n).toBe(0);
        });
    }

    return run();
}