// mocks.cpp - mock functions, test code without its slow or unreliable dependencies

#include <functional>
#include <string>

#include "jest-cpp.hpp"

using namespace jest;

struct Order {
    std::string id;
    double total;
};

// the code under test takes its dependencies as parameters, so a test can pass mocks instead of the real thing
class Checkout {
public:
    using ChargeFn = std::function<bool(double)>;
    using NotifyFn = std::function<void(const std::string&)>;

    Checkout(ChargeFn charge_, NotifyFn notify_) : charge(std::move(charge_)), notify(std::move(notify_)) {}

    bool pay(const Order& order) {
        for (int attempt = 0; attempt < 2; attempt++) {
            if (charge(order.total)) {
                notify("paid " + order.id);
                return true;
            }
        }
        notify("payment failed for " + order.id);
        return false;
    }

private:
    ChargeFn charge;
    NotifyFn notify;
};

// wraps a mock in a small class that implements it
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

std::string receipt(PaymentGateway& gateway, double amount) {
    return gateway.charge(amount) ? "receipt" : "declined";
}

int main(int argc, char** argv) {
    init(argc, argv);

    describe("Checkout") {
        auto charge = fn<bool(double)>().mockName("charge");
        auto notify = fn<void(const std::string&)>().mockName("notify");

        Checkout checkout(charge, notify); // the mocks are copied in, and keep recording into the same place
        beforeEach {
            charge.mockReset();
            notify.mockReset();
        };

        it("charges the total and says thanks") {
            charge.mockReturnValue(true);
            expect(checkout.pay({"A1", 49.99})).toBe(true);

            expect(charge).toHaveBeenCalledTimes(1);
            expect(charge).toHaveBeenCalledWith(49.99);
            expect(notify).toHaveBeenCalledWith("paid A1");
        };

        it("retries once when the first charge fails") {
            charge.mockReturnValueOnce(false).mockReturnValueOnce(true); // first call false, second true
            expect(checkout.pay({"B2", 10.0})).toBe(true);
            expect(charge).toHaveBeenCalledTimes(2);
        };

        it("gives up after the retry") {
            charge.mockReturnValue(false);

            expect(checkout.pay({"C3", 5.0})).toBe(false);
            expect(charge).toHaveBeenCalledTimes(2);
            expect(notify).toHaveBeenLastCalledWith("payment failed for C3");
            expect(notify).never.toHaveBeenCalledWith("paid C3");
        };

        it("can ignored arguments it doesn't care about") {
            charge.mockReturnValue(true);
            checkout.pay({"D4", 20.0});
            expect(notify).toHaveBeenCalledWith(anything); // 'anything' doesnt care what the arguments were, it accepts anything
        };
    }

    describe("looking at what was recorded") {
        it("keeps every call and result") {
            // this mock runs real code
            auto add = fn([](int a, int b) {
                return a + b;
            });

            add(1, 2);
            add(10, 20);

            expect(add.mock.calls).toHaveLength(2);
            expect(std::get<0>(add.mock.calls[1])).toBe(10); // first argument of the second call
            expect(add.mock.results[1].value).toEqual(30); // what the second call returned
            expect(add).toHaveReturnedWith(3);
        };
    }

    describe("mocking an interface") {
        it("wraps a mock in a class that implements it") {
            FakeGateway gateway;
            gateway.chargeMock.mockReturnValue(false);

            expect(receipt(gateway, 12.5)).toBe("declined");
            expect(gateway.chargeMock).toHaveBeenCalledWith(12.5);
        };
    }

    return run();
}