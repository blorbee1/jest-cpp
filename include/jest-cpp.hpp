/*
 * jest-cpp.hpp
 * A single-header JestJS C++ port
 *
 * Copyright (c) 2026
 * Licensed under the MIT License
 *
 * Requires C++20 or later
 */

#pragma once

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cmath>
#include <concepts>
#include <exception>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace jest {
    struct Regex : std::regex {
        std::string source;
        explicit Regex(std::string src, std::regex::flag_type f = std::regex::ECMAScript) : std::regex(src, f), source(std::move(src)) {}
    };
    inline namespace literals {
        inline Regex operator""_re(const char* s, std::size_t n) {
            return Regex(std::string(s, n));
        }
    }

    namespace detail {
        struct ExpectationFailed : std::runtime_error {
            using std::runtime_error::runtime_error;
        };

        // type traits / concepts
        template <class T>
        struct IsOptional : std::false_type {};
        template <class U>
        struct IsOptional<std::optional<U>> : std::true_type {};
        template <class T>
        inline constexpr bool is_optional_v = IsOptional<std::remove_cvref_t<T>>::value;

        template <class T> 
        concept StringLike = std::is_convertible_v<const T&, std::string_view>;
        template <class T> 
        concept Streamable = requires(std::ostream& o, const T& v) { o << v; };
        template <class T> 
        concept Range = requires(const T& v) { std::begin(v); std::end(v); };
        template <class T> 
        concept TupleLike = requires { std::tuple_size<std::remove_cvref_t<T>>::value; };
        template <class T> 
        concept PointerLike = !StringLike<T> && !is_optional_v<T> && !std::is_function_v<std::remove_pointer_t<T>> && requires(const T& p) { *p; static_cast<bool>(p); };
        
        // integers and std::cmp_* accepts (no bool / char types)
        template <class T>
        concept StdInt = std::is_integral_v<T> && !std::is_same_v<T, bool> && !std::is_same_v<T, char> && !std::is_same_v<T, wchar_t> && !std::is_same_v<T, char8_t> && !std::is_same_v<T, char16_t> && !std::is_same_v<T, char32_t>;

        // pretty printing
        template <class T> 
        std::string str(const T& v) {
            if constexpr (std::is_same_v<T, bool>) return v ? "true" : "false";
            else if constexpr (StringLike<T>) return "\"" + std::string(std::string_view(v)) + "\"";
            else if constexpr (std::is_null_pointer_v<T>) return "nullptr";
            else if constexpr (std::is_same_v<T, std::nullopt_t>) return "nullopt";
            else if constexpr (is_optional_v<T>) return v.has_value() ? "Optional(" + str(*v) + ")" : std::string("nullopt");
            else if constexpr (std::is_floating_point_v<T>) {
                char buf[64];
                auto r = std::to_chars(buf, buf + sizeof buf, v); // shortest round trip: 0.30000000000000004
                return std::string(buf, r.ptr);
            } else if constexpr (std::is_enum_v<T>) return str(static_cast<std::underlying_type_t<T>>(v));
            else if constexpr (Streamable<T>) { 
                std::ostringstream o; 
                o << v; 
                return o.str();
            } else if constexpr (Range<T>) {
                std::string out = "[";
                bool first = true;
                for (const auto& e : v) { 
                    if (!first)
                        out += ", ";
                    first = false;
                    out += str(e);
                }
                return out + "]";
            } else if constexpr (TupleLike<T>) {
                std::string out = "(";
                std::size_t i = 0;
                std::apply([&](const auto&... e) { ((out += (i++ ? ", " : "") + str(e)), ...); }, v);
                return out + ")";
            } else return "<unprintable>";
        }

        // comparison helpers

        // that is safe for mixed signed/unsigned integers
        template <class A, class B>
        bool cmpEq(const A& a, const B& b) {
            if constexpr (StdInt<A> && StdInt<B>) return std::cmp_equal(a, b);
            else if constexpr (std::is_arithmetic_v<A> && std::is_arithmetic_v<B>) return static_cast<long double>(a) == static_cast<long double>(b);
            else {
                static_assert(requires { a == b; }, "jest-cpp: these types need an operator== to be compared");
                return a == b;
            }
        }

        template <class A, class B>
        bool cmpLt(const A& a, const B& b) {
            if constexpr (StdInt<A> && StdInt<B>) return std::cmp_less(a, b);
            else if constexpr (std::is_arithmetic_v<A> && std::is_arithmetic_v<B>) return static_cast<long double>(a) < static_cast<long double>(b);
            else return a < b;
        }

        // Object.is semantics for numbers: NaN equals NaN, +0 != -0
        template <class A, class B>
        bool sameValue(const A& a, const B& b) {
            if constexpr (std::is_arithmetic_v<A> && std::is_arithmetic_v<B> && (std::is_floating_point_v<A> || std::is_floating_point_v<B>)) {
                const auto x = static_cast<long double>(a), y = static_cast<long double>(b);
                if (std::isnan(x) && std::isnan(y)) return true;
                if (!(x == y)) return false;
                return x != 0 || std::signbit(x) == std::signbit(y);
            } else return cmpEq(a, b);
        }

        // recursive equality (toEqual), looks through optionals, points, and containers
        template <class A, class B>
        bool deepEq([[maybe_unused]] const A& a, [[maybe_unused]] const B& b) {
            if constexpr (std::is_same_v<A, std::nullopt_t> || std::is_same_v<B, std::nullopt_t>) return sameValue(a, b);
            else if constexpr (is_optional_v<A> && is_optional_v<B>) return a.has_value() == b.has_value() && (!a.has_value() || deepEq(*a, *b));
            else if constexpr (is_optional_v<A>) return a.has_value() && deepEq(*a, b);
            else if constexpr (is_optional_v<B>) return b.has_value() && deepEq(a, *b);
            else if constexpr (PointerLike<A> && PointerLike<B>) {
                if (!a || !b) return !a && !b;
                return deepEq(*a, *b);
            } else if constexpr (StringLike<A> && StringLike<B>) return std::string_view(a) == std::string_view(b);
            else if constexpr (Range<A> && Range<B>) {
                auto ia = std::begin(a); auto ea = std::end(a);
                auto ib = std::begin(b); auto eb = std::end(b);
                for (; ia != ea && ib != eb; ++ia, ++ib)
                    if (!deepEq(*ia, *ib)) return false;
                return ia == ea && ib == eb;
            } else if constexpr (TupleLike<A> && TupleLike<B>) {
                constexpr std::size_t N = std::tuple_size_v<A>;
                if constexpr (N != std::tuple_size_v<B>) return false;
                else return [&]<std::size_t... I>(std::index_sequence<I...>) {
                    return (deepEq(std::get<I>(a), std::get<I>(b)) && ...);
                }(std::make_index_sequence<N>{});
            } else return sameValue(a, b);
        }

        // optional -> has value, points -> non-null, anything else is always "defined"
        template <class T>
        bool isEmpty([[maybe_unused]] const T& v) {
            if constexpr (is_optional_v<T>) return !v.has_value();
            else if constexpr (std::is_null_pointer_v<T>) return true;
            else if constexpr (StringLike<T>) return false;
            else if constexpr (requires {{ v == nullptr } -> std::convertible_to<bool>; }) return v == nullptr;
            else return false;
        }

        template <class T>
        bool truthy(const T& v) {
            if constexpr (std::is_floating_point_v<T>) return !std::isnan(v) && v != 0;
            else if constexpr (std::is_constructible_v<bool, const T&>) return static_cast<bool>(v);
            else if constexpr (StringLike<T>) return !std::string_view(v).empty();
            else return true;
        }

        // matchers
        template <class T> 
        struct Matchers {
            const T& actual;
            bool negate;
            
            Matchers(const T& act, bool neg) : actual(act), negate(neg) {}

            void check(bool pass, const std::string& matcher, const std::string& received) const {
                if (pass == negate) {
                    throw ExpectationFailed(std::string("expect(received)") + (negate ? ".never." : ".") + matcher + "\n\tReceived: " + received);
                }
            }

            // equality

            // toBe: shallow. Pointers compare by address; floats use Object.is rules (NaN==NaN, +0 != -0)
            template <class U> 
            void toBe(const U& expected) const {
                check(sameValue(actual, expected), "toBe(" + str(expected) + ")", str(actual));
            }

            // toEqual: depp. Looks through optionals, pointers/smart pointers, containers, and tuples
            template <class U> 
            void toEqual(const U& expected) const {
                check(deepEq(actual, expected), "toEqual(" + str(expected) + ")", str(actual));
            }

            // truthiness / emptiness
            void toBeTruthy() const { check(truthy(actual), "toBeTruthy()", str(actual)); }
            void toBeFalsy() const { check(!truthy(actual), "toBeFalsy()", str(actual)); }
            void toBeDefined() const { check(!isEmpty(actual), "toBeDefined()", str(actual)); }
            void toBeUndefined() const { check(isEmpty(actual), "toBeUndefined()", str(actual)); }
            void toBeNull() const { check(isEmpty(actual), "toBeNull()", str(actual)); }

            // numbers
            template <class U> 
            void toBeGreaterThan(const U& e) const {
                check(cmpLt(e, actual), "toBeGreaterThan(" + str(e) + ")", str(actual));
            }
            template <class U> 
            void toBeGreaterThanOrEqual(const U& e) const {
                check(cmpLt(e, actual) || cmpEq(actual, e), "toBeGreaterThanOrEqual(" + str(e) + ")", str(actual));
            }

            template <class U> 
            void toBeLessThan(const U& e) const {
                check(cmpLt(actual, e), "toBeLessThan(" + str(e) + ")", str(actual));
            }
            template <class U> 
            void toBeLessThanOrEqual(const U& e) const {
                check(cmpLt(actual, e) || cmpEqa(actual, e), "toBeLessThanOrEqual(" + str(e) + ")", str(actual));
            }

            void toBeCloseTo(double expected, int digits = 2) const {
                static_assert(std::is_arithmetic_v<T>, "toBeCloseTo needs a number");
                const double a = static_cast<double>(actual);
                const bool pass = (std::isinf(a) || std::isinf(expected))
                    ? a == expected
                    : std::fabs(expected - a) < std::pow(10.0, -digits) / 2.0;
                check(pass, "toBeCloseTo(" + str(expected) + ", " + std::to_string(digits) + ")", str(actual));
            }

            // strings and containers
            template <class U>
            void toContain(const U& item) const {
                bool found = false;
                if constexpr (StringLike<T>) {
                    if constexpr (StringLike<U>) found = std::string_view(actual).find(std::string_view(item)) != std::string_view::npos;
                    else found = std::string_view(actual).find(item) != std::string_view::npos;
                } else {
                    found = std::any_of(std::begin(actual), std::end(actual), [&](const auto& e) {
                        return sameValue(e, item);
                    });
                }
                check(found, "toContain(" + str(item) + ")", str(actual));
            }

            void toHaveLength(size_t n) const {
                size_t len;
                if constexpr (StringLike<T>) len = std::string_view(actual).size();
                else len = static_cast<size_t>(std::distance(std::begin(actual), std::end(actual)));
                check(len == n, "toHaveLength(" + std::to_string(n) + ")", std::to_string(len));
            }
            void toHaveLength(unsigned int n) const {
                toHaveLength(static_cast<size_t>(n));
            }
            void toHaveLength(int n) const {
                if (n < 0)
                    throw std::runtime_error("Invalid length");
                toHaveLength(static_cast<size_t>(n));
            }

            // toMatch(regex) searches the regular expression
            void toMatch(const Regex& re) const {
                static_assert(StringLike<T>, "toMatch needs a string");
                const std::string s = std::string(std::string_view(actual));
                check(std::regex_search(s, re), "toMatch(/" + re.source + "/)", str(actual));
            }

            // toMatch(regex) searches the regular expression
            void toMatch(const std::regex& re) const {
                static_assert(StringLike<T>, "toMatch needs a string");
                const std::string s = std::string(std::string_view(actual));
                check(std::regex_search(s, re), "toMatch(<regex>)", str(actual));
            }

            // toMatch(string) checks for substring
            void toMatch(const std::string& sub) const {
                static_assert(StringLike<T>, "toMatch needs a string");
                check(std::string_view(actual).find(sub) != std::string_view::npos, "toMatch(" + str(sub) + ")", str(actual));
            }
            // toMatch(string) checks for substring
            void toMatch(const char* sub) const {
                toMatch(std::string(sub));
            }

            // callables
            void toThrow() const {
                bool threw = false;
                try {
                    actual();
                } catch (...) {
                    threw = true;
                }
                check(threw, "toThrow()", threw ? "threw" : "did not throw");
            }

            void toThrow(const std::string& substr) const {
                bool ok = false;
                std::string what = "did not throw";
                try {
                    actual();
                } catch (std::exception& e) {
                    what = std::string("threw \"") + e.what() = "\"";
                    ok = std::string_view(e.what()).find(substr) != std::string::npos;
                } catch (...) {
                    what = "threw non-std exception";
                }
                check(ok, "toThrow(" + str(substr) + ")", what);
            }
        };

        template <class T>
        struct ValueHolder {
            T held;
            explicit ValueHolder(T&& v) : held(std::move(v)) {}
        };

        template <class T>
        struct Expectation : private ValueHolder<T>, public Matchers<T> {
            Matchers<T> never; // "not" is reserved keyword
            explicit Expectation(T v) : ValueHolder<T>(std::move(v)), Matchers<T>{this->held, false}, never{this->held, true} {}
            Expectation(const Expectation&) = delete;
            Expectation& operator=(const Expectation&) = delete;
        };

        // suite tree
        using Hooks = std::vector<std::function<void()>>;
        struct Test {
            std::string name;
            std::function<void()> fn;
            int timeoutMs; // -1 = use default
            bool expectFail; // test.failing
        };
        struct Suite {
            std::string name;
            Hooks before_all, after_all, before_each, after_each;
            std::vector<Test> tests;
            std::vector<std::unique_ptr<Suite>> children;
            std::vector<std::shared_ptr<const void>> keepAlive; // data that describe.each bodies refer to
        };

        inline Suite& root() {
            static Suite r;
            return r;
        }
        inline Suite*& current() {
            static Suite* c = &root();
            return c;
        }

        inline void addTest(std::string name, std::function<void()> fn, int timeoutMs, bool expectFail) {
            current()->tests.push_back(Test{std::move(name), std::move(fn), timeoutMs, expectFail});
        }\

        // RAII: entering a describe block pushes a suite, leaving it pops
        struct DescribeScope {
            Suite* prev;
            explicit DescribeScope(const std::string& name) : prev(current()) {
                auto s = std::make_unique<Suite>();
                s->name = name;
                Suite* raw = s.get();
                prev->children.push_back(std::move(s));
                current() = raw;
            }
            ~DescribeScope() {
                current() = prev;
            }
            DescribeScope(const DescribeScope&) = delete;
            DescribeScope& operator=(const DescribeScope&) = delete;
        };

        // Registrar{"name"} = [&]() {...}; -> stores the lambda for later
        struct TestRegistrar {
            std::string name;
            int timeoutMs;
            explicit TestRegistrar(std::string n, int timeout = -1) : name(std::move(n)), timeoutMs(timeout) {}
            void operator=(std::function<void()> fn) {
                addTest(name, std::move(fn), timeoutMs, false);
            }
        };
        struct HookRegistrar {
            Hooks Suite::*slot;
            void operator=(std::function<void()> fn) {
                (current()->*slot).push_back(std::move(fn));
            }
        };

        // .each / .failing
        struct Arg {
            std::string raw, quoted;
        };
        template <class T>
        Arg makeArg(const T& v) {
            Arg a;
            a.quoted = str(v);
            if constexpr (StringLike<T>) a.raw = std::string(std::string_view(v));
            else a.raw = a.quoted;
            return a;
        }

        // printf-style titled: %s, %d, %i, %f (plain), %j %o %p (quoted), %# (row index), %$ (row number), etc
        inline std::string formatTitle(const std::string& tpl, const std::vector<Arg>& args, std::size_t index) {
            std::string out;
            std::size_t next = 0;
            for (std::size_t i = 0; i < tpl.size(); ++i) {
                const char ch = tpl[i];
                if (ch == '%' && i + 1 < tpl.size()) {
                    const char k = tpl[i + 1];
                    if (k == '%') { out += '%'; ++i; continue; }
                    if (k == '#') { out += std::to_string(index); ++i; continue; }
                    if (k == '$') { out += std::to_string(index + 1); ++i; continue; }
                    if ((k == 's' || k == 'd' || k == 'i' || k == 'f' || k == 'j' || k == 'o' || k =='p') && next < args.size()) {
                        const Arg& a = args[next++];
                        out += (k == 'j' || k == 'o' || k == 'p') ? a.quoted : a.raw;
                        ++i;
                        continue;
                    }
                }
                out += ch;
            }
            return out;
        }

        template <class Row>
        std::vector<Arg> rowArgs(const Row& row) {
            if constexpr (TupleLike<Row>)
                return std::apply([](const auto&... e) { return std::vector<Arg>{makeArg(e)...}; }, row);
            else return {makeArg(row)};
        }
        // tuple-like rows (tuple, pair, array) are spread into the callback's parameters
        template <class F, class Row>
        void callWithRow(F& f, const Row& row) {
            if constexpr (TupleLike<Row>) std::apply(f, row);
            else f(row);
        }

        // tests registered inside a describe.each body capture by reference, so the callback's parameters must be references into data that outlives the call
        template <class M>
        struct MemFnTraits;
        template <class C, class R, bool N, class... A>
        struct MemFnTraits<R (C::*)(A...) const noexcept(N)> {
            static constexpr bool refsOnly = (std::is_reference_v<A> && ...);
        };
        template <class C, class R, bool N, class... A>
        struct MemFnTraits<R (C::*)(A...) noexcept(N)> {
            static constexpr bool refsONly = (std::is_reference_v<A> && ...);
        };

        template <class F>
        constexpr bool takesRefsOnly() {
            using G = std::remove_cvref_t<F>;
            if constexpr (requires { &G::operator(); }) return MemFnTraits<decltype(&G::operator())>::refsOnly;
            else return true; // generic lamdas / function pointers: cant tell
        }

        template <class Row>
        class EachBinder {
        private:
            std::vector<Row> rows;
            bool failing;

        public:
            EachBinder(std::vector<Row> r, bool fail) : rows(std::move(r)), failing(fail) {}
            template <class F>
            void operator()(const std::string& name, F&& fn, int timeoutMs = -1) const {
                auto f = std::make_shared<std::decay_t<F>>(std::forward<F>(fn));
                for (std::size_t i = 0; i < rows.size(); ++i) {
                    Row row = rows[i];
                    addTest(formatTitle(name, rowArgs(row), i), [f, row] { callWithRow(*f, row); }, timeoutMs, failing);
                }
            }
        };

        template <class Row>
        class DescribeEachBinder {
        private:
            std::vector<Row> rows;

        public:
            DescribeEachBinder(std::vector<Row> r, bool) : rows(std::move(r)) {}
            template <class F>
            void operator()(const std::string& name, F&& fn, int /*timeoutMs, unsued*/ = -1) const {
                static_assert(takesRefsOnly<F>(), "describe.each: take the row parameters as const references (e.g. 'const std::string& s')");
                auto rows_ = std::make_shared<const std::vector<Row>>(rows);
                current()->keepAlive.push_back(rows_);
                for (std::size_t i = 0; i < rows_->size(); ++i) {
                    DescribeScope scope{formatTitle(name, rowArgs((*rows_)[i]), i)};
                    callWithRow(fn, (*rows_)[i]);
                }
            }
        };

        // {"a", "b"} gives const char* rows; store them as std::string so callbacks can take it
        template <class T>
        using RowOf = std::conditional_t<std::is_pointer_v<T> && std::is_same_v<std::remove_cv_t<std::remove_pointer_t<T>>, char>, std::string, T>;

        template <template <class> class Binder>
        struct EachFn {
            bool failing;
            template <class R>
            requires Range<R> auto operator()(const R& rows) const {
                using Row = RowOf<std::decay_t<decltype(*std::begin(rows))>>;
                return Binder<Row>(std::vector<Row>(std::begin(rows), std::end(rows)), failing);
            }
            template <class T>
            auto operator()(std::initializer_list<T> rows) const {
                using Row = RowOf<T>;
                return Binder<Row>(std::vector<Row>(rows.begin(), rows.end()), failing);
            }
        };

        struct FailingApi {
            EachFn<EachBinder> each{true};
            void operator()(const std::string& name, std::function<void()> fn, int timeoutMs = -1) const {
                addTest(name, std::move(fn), timeoutMs, true);
            }
        };

        struct TestApi {
            EachFn<EachBinder> each{false};
            FailingApi failing{};
            void operator()(const std::string& name, std::function<void()> fn, int timeoutMs = -1) const {
                addTest(name, std::move(fn), timeoutMs, false);
            }
        };

        struct DescribeApi {
            EachFn<DescribeEachBinder> each{false};
            void operator()(const std::string& name, const std::function<void()>& fn) const {
                DescribeScope scope{name};
                fn();
            }
        };

        // command line + runner
        struct Options {
            bool verbose = false;
            bool color = true;
            int timeoutMs = 5000;
            std::string pattern; // regex on the full test name ("describe nested test"); empty = all
        };

        inline void printUsage(std::ostream& os, const std::string& prog) {
            os << "Usage: " << prog << " [options]\n\n"
                << "Options:\n"
                << "  -v, --verbose                  print every test result, grouped by describe block\n"
                << "  -t, --testNamePattern <regex>  only run tests whose full name matches the regex\n"
                << "      --timeout <ms>             default per-test timeout (default 5000)\n"
                << "      --no-color                 disable colored output (also honors NO_COLOR)\n"
                << "  -h, --help                     show this help\n";
        }

        // returns an exit code if the program should stop right away
        inline std::optional<int> parseArgs(const std::vector<std::string>& args, const std::string& prog, Options& o) {
            auto bad = [&](const std::string& msg) {
                std::cerr << prog << ": " << msg << "\n\n";
                printUsage(std::cerr, prog);
                return std::optional<int>(2);
            };

            for (std::size_t i = 0 ; i < args.size(); ++i) {
                std::string key = args[i], val;
                bool hasVal = false;
                if (key.rfind("--", 0) == 0) {
                    if (auto eq = key.find('='); eq != std::string::npos) {
                        val = key.substr(eq + 1);
                        key.resize(eq);
                        hasVal = true;
                    }
                }

                auto needValue = [&] {
                    if (hasVal) return true;
                    if (i + 1 < args.size()) {
                        val = args[++i];
                        return true;
                    }
                    return false;
                };
                
                if (key == "-v" || key == "--verbose") o.verbose = true;
                else if (key == "--no-color") o.color = false;
                else if (key == "-h" || key == "--help") { printUsage(std::cout, prog); return 0; }
                else if (key == "-t" || key == "--testNamePattern") {
                    if (!needValue()) return bad("option '" + key + "' needs a value");
                    o.pattern = val;
                } else if (key == "--timeout") {
                    if (!needValue()) return bad("option '--timeout' needs a value");
                    try {
                        o.timeoutMs = std::stoi(val);
                    } catch (...) {
                        return bad("invalid --timeout value '" + val + "'");
                    }
                } else return bad("unknown option '" + args[i] + "'");
            }
            return std::nullopt;
        }
        
        inline std::string indentLines(const std::string& text, const std::string& pad) {
            std::string out = pad;
            for (char ch : text) {
                out += ch;
                if (ch == '\n')
                    out += pad;
            }
            return out;
        }

        class Runner {
        private:
            using Clock = std::chrono::steady_clock;
            Options opts;
            std::regex re;
            bool hasRe = false;
            std::ostream& out = std::cout;
            std::vector<const Suite*> chain; // root .. current suite
            std::size_t headingsPrinted = 1; // verbose: how many chain entries have had their heading printed
            int passed = 0, failed = 0, skipped = 0;

            static constexpr const char *GREEN = "\033[32m",
                                        *RED = "\033[31m",
                                        *YELLOW = "\033[33m",
                                        *DIM = "\033[2m",
                                        *RESET = "\033[0m";

            std::string paint(const char* code, const std::string& s) const {
                return opts.color ? std::string(code) + s + RESET : s;
            }
            
            static std::string join(const std::string& prefix, const std::string& name) {
                return prefix.empty() ? name : prefix + " " + name;
            }

            bool selected(const std::string& fullName) const {
                return !hasRe || std::regex_search(fullName, re);
            }
            bool anySelected(const Suite& s, const std::string& fullName) const {
                for (const Test& t : s.tests) if (selected(join(fullName, t.name))) return true;
                for (const auto& c : s.children) if (anySelected(*c, join(fullName, c->name))) return true;
                return false;
            }

            static int countTests(const Suite& s) {
                int n = static_cast<int>(s.tests.size());
                for (const auto& c : s.children) n += countTests(*c);
                return n;
            }

            void flushHeadings() {
                for (; headingsPrinted < chain.size(); ++headingsPrinted)
                    out << std::string(2 * (headingsPrinted - 1), ' ') << chain[headingsPrinted]->name << '\n';
            }

            void report(bool testPassed, const std::string& name, const std::string& message, long long ms) {
                testPassed ? ++passed : ++failed;
                if (opts.verbose) {
                    flushHeadings();
                    const std::string pad(2 * (chain.size() - 1), ' ');
                    out << pad << paint(testPassed ? GREEN : RED, testPassed ? "PASS" : "FAIL") << ' ' 
                        << (testPassed ? paint(DIM, name) : paint(RED, name));
                    if (ms >= 1) out << paint(DIM, " (" + std::to_string(ms) + " ms)");
                    out << '\n';
                    if (!testPassed) out << indentLines(message, pad + "\t") << '\n';
                } else if (!testPassed) {
                    std::string path;
                    for (std::size_t i = 1; i < chain.size(); ++i) path += chain[i]->name + " > ";
                    out << paint(RED, "FAIL " + path + name) << '\n' << indentLines(message, "\t") << "\n\n";
                }
            }

            void runHooks(const Hooks& hooks, const std::string& label) {
                for (const auto& h : hooks) {
                    try {
                        h();
                    } catch (const std::exception& e) {
                        report(false, label, std::string("Unexpected exception: ") + e.what(), 0);
                    } catch (...) {
                        report(false, label, "Unexpected non-std exception", 0);
                    }
                }
            }

            void runTest(const Test& t) {
                const int timeout = t.timeoutMs >= 0 ? t.timeoutMs : opts.timeoutMs;
                bool testFailed = false;
                std::string error;
                
                auto fail = [&](std::string msg) {
                    if (!testFailed) {
                        testFailed = true;
                        error = std::move(msg);
                    }
                };
                auto guarded = [&](const auto& f) {
                    try {
                        f();
                    } catch (const ExpectationFailed& e) {
                        fail(e.what());
                    } catch (const std::exception& e) {
                        fail(std::string("Unexpected exception: ") + e.what());
                    } catch (...) {
                        fail("Unexpected non-std exception");
                    }
                };

                long long ms = 0;
                guarded([&] {
                    for (const Suite* c : chain)
                        for (const auto& h : c->before_each) h();
                });

                if (!testFailed) {
                    const auto start = Clock::now();
                    guarded(t.fn);
                    ms = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count();
                    if (!testFailed && ms > timeout)
                        fail("Exceeded tiemout of " + std::to_string(timeout) + " ms for a test (took " + std::to_string(ms) + " ms).");
                }
                
                guarded([&] {
                    for (auto rit = chain.rbegin(); rit != chain.rend(); ++rit)
                        for (const auto& h : (*rit)->after_each) h();
                });

                if (t.expectFail) {
                    report(testFailed, t.name, testFailed ? "" : "Failing test passed even though it was supposed to fail. Remove '.failing' to remove error.", ms);
                } else {
                    report(!testFailed, t.name, error, ms);
                }
            }

            void runSuite(const Suite& s, const std::string& fullName) {
                if (!anySelected(s, fullName)) { skipped += countTests(s); return; }
                chain.push_back(&s);
                runHooks(s.before_all, "beforeAll");

                for (const Test& t : s.tests) {
                    if (!selected(join(fullName, t.name))) {
                        ++skipped;
                        continue;
                    }
                    runTest(t);
                }
                for (const auto& c : s.children) runSuite(*c, join(fullName, c->name));

                runHooks(s.after_all, "afterAll");
                chain.pop_back();
                if (headingsPrinted > chain.size()) headingsPrinted = chain.size();
            }

        public:
            explicit Runner(Options o) : opts(std::move(o)) {}

            int run() {
                if (!opts.pattern.empty()) {
                    try {
                        re = std::regex(opts.pattern);
                        hasRe = true;
                    } catch (const std::regex_error& e) {
                        std::cerr << "Invalid --testNamePattern regex: " << e.what() << '\n';
                        return 2;
                    }
                }

                const auto start = Clock::now();
                runSuite(root(), "");
                const double secs = std::chrono::duration<double>(Clock::now() - start).count();

                if (opts.verbose)
                    out << '\n'; // failure blocks in quiet mode already end with a blank line

                out << "Tests:  "
                    << paint(RED, std::to_string(failed) + " failed") << ", "
                    << paint(GREEN, std::to_string(passed) + " passed")
                    << (skipped ? ", " + paint(YELLOW, std::to_string(skipped) + " skipped") : "")
                    << ", " << (passed + failed + skipped) << " total\n"
                    << "Time:   " << std::fixed << std::setprecision(3) << secs << " s\n";
                return failed ? 1 : 0;
            }
        };
    }

    // public API
    inline const detail::TestApi it{}, test{};
    inline const detail::DescribeApi describe{};

    template <class T>
    auto expect(T&& value) {
        if constexpr (std::is_convertible_v<T, const char*> && !std::is_same_v<std::decay_t<T>, std::nullptr_t>)
            return detail::Expectation<std::string>(std::string(value)); // string literals -> std::string
        else
            return detail::Expectation<std::decay_t<T>>(std::forward<T>(value));
    }

    // rows for it.each / describe.each when every row is a different typed tuple:
    //   it.each(table<int, int, int>({{1, 1, 2}, {1, 2, 3}}))("%d + %d = %d", [](int a, int b, int sum) { ... });
    template <class... Ts>
    std::vector<std::tuple<Ts...>> table(std::initializer_list<std::tuple<Ts...>> rows) {
        return std::vector<std::tuple<Ts...>>(rows);
    }

    // runs everything registered, pass main's argc/argv to enable the command line options
    // returns 0 if all tests passed, 1 if any failed, 2 for usage error
    inline int run(int argc = 0, char** argv = nullptr) {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) args.emplace_back(argv[i]);
        detail::Options opts;
        if (const char* nc = std::getenv("NO_COLOR"); nc && *nc) opts.color = false;
        if (auto code = detail::parseArgs(args, argc > 0 ? argv[0] : "tests", opts)) return *code;
        return detail::Runner(std::move(opts)).run();
    }
}

// define JEST_NO_MACROS before including to not define the macros
#ifndef JEST_NO_MACROS

#define JEST_CAT_(a, b) a##b
#define JEST_CAT(a, b) JEST_CAT_(a, b)
// describe("name") { ... }
#define describe(name) if (::jest::detail::DescribeScope JEST_CAT(jest_scope_, __COUNTER__){name}; true)

// it("name") { ... };
#define it(...) ::jest::detail::TestRegistrar{__VA_ARGS__} = [&]()
// test("name") { ... };
#define test(...) it(__VA_ARGS__)

// beforeAll { ... };
#define beforeAll ::jest::detail::HookRegistrar{&::jest::detail::Suite::before_all} = [&]()
// afterAll { ... };
#define afterAll ::jest::detail::HookRegistrar{&::jest::detail::Suite::after_all} = [&]()
// beforeEach { ... };
#define beforeEach ::jest::detail::HookRegistrar{&::jest::detail::Suite::before_each} = [&]()
// afterEach { ... };
#define afterEach ::jest::detail::HookRegistrar{&::jest::detail::Suite::after_each} = [&]()

#endif