// basics.cpp - describe / it / expect, and the shape of main()

#include <string>
#include <vector>

#include "jest-cpp.hpp" // include jest-cpp AFTER your other headers to prevent macro issues

using namespace jest;

class Counter {
    int n = 0;
public:
    void inc() { ++n; }
    int value() const { return n; }
};

int main(int argc, char** argv) {
    init(argc, argv); // parse command line options, call BEFORE the first describe or it/test

    describe("Counter") {
        Counter c; // plain locals in a describe body are fine, tests run right where they are written

        it("starts at zero") {
            expect(c.value()).toBe(0);
        }; // note the trailing semicolon after every `it`

        it("counts") {
            c.inc();
            c.inc();
            expect(c.value()).toBe(2);
            expect(c.value()).never.toBe(3); // `never` is JestJS's `not`
        };
    }

    describe("strings") {
        std::string greeting = "hello world";

        it("can be searched") {
            expect(greeting).toContain("world");
            expect(greeting).toMatch("w.r"_re); // "..."_re is a regex literal in C++
            expect(greeting).toHaveLength(11);
        };

        describe("nested describes") {
            it("sees the outer locals") {
                expect(greeting).toBe("hello world");
            };
        }
    }

    return run(); // prints the summary, returns 0 if everything passes, 1 otherwise
}