#include <stdexcept>
#include <string>
#include <vector>

#include "jest-cpp.hpp"

using namespace jest;

int add(int a, int b) {
    return a + b;
}

int main(int argc, char** argv) {
    init(argc, argv);

    // basic assertion failures
    describe("basic failures") {
        it("reports unequal values") {
            expect(add(2, 3)).toBe(6);
        };

        it("reports failed negation") {
            expect(5).never.toBe(5);
        };

        it("reports failed comparisons") {
            expect(3).toBeGreaterThan(10);
        };
    }

    // collection and string failures
    describe("matcher failures") {
        it("reports unequal vectors") {
            expect(std::vector{1, 2, 3}).toEqual(std::vector{1, 2, 4});
        };

        it("reports unmatched strings") {
            expect("hello world").toMatch("goodbye");
        };

        it("reports incorrect lengths") {
            expect("hello").toHaveLength(10);
        };
    }

    // exception matcher failures
    describe("exception failures") {
        it("fails when no exception is thrown") {
            expect([] {}).toThrow();
        };

        it("fails when the exception message differs") {
            expect([] {
                throw std::runtime_error("actual error");
            }).toThrow("expected error");
        };
    }

    // nested suite failures
    describe("nested failures") {
        describe("inner suite") {
            it("reports failures in nested tests") {
                expect(add(10, 20)).toBe(100);
            };

            it("continues executing subsequent tests") {
                expect(true).toBe(true);
            };
        }
    }

    return run();
}