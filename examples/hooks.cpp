// hooks.cpp - beforeAll / beforeEach / afterEach / afterAll

#include <string>
#include <vector>

#include "jest-cpp.hpp"

using namespace jest;

int main(int argc, char** argv) {
    init(argc, argv);

    describe("a fresh fixture for every test") {
        std::vector<int> items;

        // hooks apply to the tests that come AFTER them in the **same describe**, so declare them first
        beforeEach { items = {1, 2, 3}; };

        it("can modify the fixture") {
            items.push_back(4);
            expect(items).toHaveLength(4);
        };

        it("sees it reset") {
            expect(items).toHaveLength(3);
        };
    }

    describe("hook order") {
        std::vector<std::string> log;
        beforeAll { log.push_back("beforeAll"); }; // lazily, right before the first test that runs
        beforeEach { log.push_back("beforeEach"); };
        afterEach { log.push_back("afterEach"); };

        it("first") {};
        it("second") {
            expect(log).toEqual(std::vector<std::string>{"beforeAll", "beforeEach", "afterEach", "beforeEach"});
        };
        // afterAll { ... }; runs when the describe block exists, AFTER its locals are destroyed
        // so only touch statics or outer variables
    }

    return run();
}