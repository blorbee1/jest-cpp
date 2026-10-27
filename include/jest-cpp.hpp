/*
 * @file jest-cpp.hpp
 * @brief A single-header JestJS C++ port
 *
 * jest-cpp provides Jest style syntaax for writing and running C++ tests.
 *
 * Define `JEST_NO_MACROS` if you don't want any macros from jest-cpp
 *
 * @section usage Basic usage
 *
 * @code
 * #include "jest-cpp.hpp"
 *
 * int main(int argc, char** argv) {
 *     init(argc, argv);
 *
*      describe("math") {
 *         it("adds two numbers") {
 *             expect(1 + 2).toBe(3);
 *         };
 *     }
 *
 *     return run();
 * }
 *
 * @section requirements Requirements
 * - C++20 or later
 *
 * @copyright Copyright (c) 2026
 * @license MIT
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

/// @cond TURN_OFF_DOXYGEN

// build configuration
// optional separate compilation (much faster to compile) define JEST_CPP_SEPARATE for every file,
// and define JEST_CPP_IMPLEMENTATION (then include this header) in exactly ONE .cpp file. without JEST_CPP_SEPARATE
#if defined(JEST_CPP_SEPARATE)
#define JEST_CPP_INLINE
#else
#define JEST_CPP_INLINE inline
#ifndef JEST_CPP_IMPLEMENTATION
#define JEST_CPP_IMPLEMENTATION
#endif
#endif

#if defined(__GNUC__) || defined(__clang__)
#define JEST_CPP_COLD __attribute__((cold, noinline))
#elif defined(_MSC_VER)
#define JEST_CPP_COLD __declspec(noinline)
#else
#define JEST_CPP_COLD
#endif

/// @endcond

/**
 * @namespace jest
 * @brief Public API for the jest-cpp testing framework.
 */
namespace jest {
    /**
     * @brief A regular expression that retains its original pattern.
     *
     * Inherits from std::regex and stores the source pattern for use in assertion failure messages.
     */
    struct Regex : std::regex {
        //// Original regular expression pattern.
        std::string source;

        /**
         * @brief Constructs a regular expression.
         * @param src Regular expression pattern
         * @param f Regular expression syntax flags
         */
        explicit Regex(std::string src, std::regex::flag_type f = std::regex::ECMAScript);
    };

    inline namespace literals {
        /**
         * @brief Creates a Regex from a string literal.
         *
         * @param s Pointer to the literal's characters.
         * @param n Number of characters in the literal
         * @return A regex constructed from the literal.
         *
         * @code
         * jest::Regex pattern = "hello.*"_re;
         * @endcode
         */
        JEST_CPP_INLINE Regex operator""_re(const char* s, std::size_t n);
    }

    namespace detail {
        /// @cond TURN_OFF_DOXYGEN

        // std::regex is the most expensive thing in this header to compile, so non templated uses go through these functions
        JEST_CPP_INLINE bool regexSearch(const std::string& text, const std::regex& re);
        JEST_CPP_INLINE bool regexAssign(std::regex& out, const std::string& pattern, std::string& error);

        /// @endcond

        /**
         * @struct ExpectationFailed
         * @brief Thrown to indicate that an expectation failed its conditions
         */
        struct ExpectationFailed : std::runtime_error {
            using std::runtime_error::runtime_error;
        };

        /// @cond TURN_OFF_DOXYGEN
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
        struct MatcherBase {
            bool negate;
            explicit MatcherBase(bool neg) : negate(neg) {}

            [[noreturn]] JEST_CPP_COLD void fail(const std::string& matcher, const std::string& received) const {
                throw ExpectationFailed(std::string("expect(received).") + (negate ? "never." : "") + matcher + "\n\tReceived: " + received);
            }
        };

        /// @endcond

        template <class T> 
        struct Matchers : MatcherBase {
            /// @cond TURN_OFF_DOXYGEN

            const T& actual;
            Matchers(const T& act, bool neg) : MatcherBase(neg), actual(act) {}

            void check(bool pass, const std::string& matcher, const std::string& received) const {
                if (pass == negate) {
                    fail(matcher, received);
                }
            }

            /// @endcond

            // equality

            /**
             * @brief Checks whether the actual value equals the expected value.
             *
             * Uses Object.is like semantics for floating point values, treating NaN as equal to NaN and +0 != -0.
             * Pointers are compared by address.
             *
             * @tparam U Type of the expected value
             * @param expected Value to compare against
             * @throws detail::ExpectationFailed If the assertion fails.
             */
            template <class U>
            void toBe(const U& expected) const {
                check(sameValue(actual, expected), "toBe(" + str(expected) + ")", str(actual));
            }

            /**
             * @brief Performs deep equality.
             *
             * Deeply compares optionals by comparing their values, pointers are compared by value, so on.
             *
             * @tparam U Type of the expected value.
             * @param expected Value to compare against
             * @throws detail::ExpectationFailed If the assertion fails
             */
            template <class U>
            void toEqual(const U& expected) const {
                check(deepEq(actual, expected), "toEqual(" + str(expected) + ")", str(actual));
            }

            /**
             * @brief Checks that the value is truthy.
             * @throws detail::ExpectationFailed If the value is falsy
             */
            void toBeTruthy() const { check(truthy(actual), "toBeTruthy()", str(actual)); }

            /**
             * @brief Checks that the value is falsy.
             * @throws detail::ExpectationFailed If the value is truthy
             */
            void toBeFalsy() const { check(!truthy(actual), "toBeFalsy()", str(actual)); }

            /**
             * @brief Checks that the value is considered defined.
             *
             * Defined values only matter with std::optional and pointers, every other value is always "defined"
             * @throws detail::ExpectationFailed If the value is not defined
             */
            void toBeDefined() const { check(!isEmpty(actual), "toBeDefined()", str(actual)); }

            /**
             * @brief Checks that the value is considered undefined.

             * Defined values only matter with std::optional and pointers, every other value is always "defined"
             * @throws detail::ExpectationFailed If the value is defined
             *
             * @see toBeNull
             */
            void toBeUndefined() const { check(isEmpty(actual), "toBeUndefined()", str(actual)); }

            /**
             * @brief Alias for toBeUndefined
             * @see toBeUndefined
             */
            void toBeNull() const { check(isEmpty(actual), "toBeNull()", str(actual)); }

            /**
             * @brief Checks whether actual > expected.
             * @tparam U Type of the expected value.
             * @param expected Lower bound for the actual value.
             * @throws detail::ExpectationFailed If the comparison fails.
             */
            template <class U>
            void toBeGreaterThan(const U& expected) const {
                check(cmpLt(expected, actual), "toBeGreaterThan(" + str(expected) + ")", str(actual));
            }

            /**
             * @brief Checks whether actual >= expected.
             * @tparam U Type of the expected value.
             * @param expected Lower bound for the actual value.
             * @throws detail::ExpectationFailed If the comparison fails.
             */
            template <class U>
            void toBeGreaterThanOrEqual(const U& expected) const {
                check(cmpLt(expected, actual) || cmpEq(actual, expected), "toBeGreaterThanOrEqual(" + str(expected) + ")", str(actual));
            }

            /**
             * @brief Checks whether actual < expected.
             * @tparam U Type of the expected value.
             * @param expected Upper bound for the actual value.
             * @throws detail::ExpectationFailed If the comparison fails.
             */
            template <class U>
            void toBeLessThan(const U& expected) const {
                check(cmpLt(actual, expected), "toBeLessThan(" + str(expected) + ")", str(actual));
            }

            /**
             * @brief Checks whether actual <= expected.
             * @tparam U Type of the expected value.
             * @param expected Upper bound for the actual value.
             * @throws detail::ExpectationFailed If the comparison fails.
             */
            template <class U>
            void toBeLessThanOrEqual(const U& expected) const {
                check(cmpLt(actual, expected) || cmpEq(actual, expected), "toBeLessThanOrEqual(" + str(expected) + ")", str(actual));
            }

            /**
             * @brief Checks whether a floating point number is close to an expected value.
             * @param expected Expected numeric value.
             * @param digits Number of decimal digits used to determine tolerance.
             * @throws detail::ExpectationFailed If the values are not sufficiently close.
             */
            void toBeCloseTo(double expected, int digits = 2) const {
                static_assert(std::is_arithmetic_v<T>, "toBeCloseTo needs a number");
                const double a = static_cast<double>(actual);
                const bool pass = (std::isinf(a) || std::isinf(expected))
                    ? a == expected
                    : std::fabs(expected - a) < std::pow(10.0, -digits) / 2.0;
                check(pass, "toBeCloseTo(" + str(expected) + ", " + std::to_string(digits) + ")", str(actual));
            }

            /**
             * @brief Checks whether a string contains a substring or a range contains an item.
             *
             * Ranges are anything that has std::begin/std::end defined
             *
             * @tparam U Type of the item to find
             * @param item Item or substring to search for.
             * @throws detail::ExpectationFailed If the item is not found.
             */
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

            /**
             * @brief Checks the length of a string or range.
             * @param n Expected length.
             * @throws detail::ExpectationFailed If the length differs.
             */
            void toHaveLength(size_t n) const {
                size_t len;
                if constexpr (StringLike<T>) len = std::string_view(actual).size();
                else len = static_cast<size_t>(std::distance(std::begin(actual), std::end(actual)));
                check(len == n, "toHaveLength(" + std::to_string(n) + ")", std::to_string(len));
            }
            void toHaveLength(unsigned int n) const {
                toHaveLength(static_cast<size_t>(n));
            }

            /**
             * @throws std::runtime_error If the supplied integer length is negative
             */
            void toHaveLength(int n) const {
                if (n < 0)
                    throw std::runtime_error("Invalid length");
                toHaveLength(static_cast<size_t>(n));
            }

            /**
             * @brief Checks whether a string contains a match for a regular expression.
             * @param re Regular expression to search for.
             * @throws detail::ExpectationFailed If no match is found.
             */
            void toMatch(const Regex& re) const {
                static_assert(StringLike<T>, "toMatch needs a string");
                const std::string s = std::string(std::string_view(actual));
                check(regexSearch(s, re), "toMatch(/" + re.source + "/)", str(actual));
            }

            /**
             * @brief Checks whether a string contains a match for a standard regex.
             * @param re Regular expression to search for.
             * @throws detail::ExpectationFailed If no match is found,
             */
            void toMatch(const std::regex& re) const {
                static_assert(StringLike<T>, "toMatch needs a string");
                const std::string s = std::string(std::string_view(actual));
                check(regexSearch(s, re), "toMatch(<regex>)", str(actual));
            }

            /**
             * @brief Checks whether a string contains the specifided substring.
             * @param sub Substring to find.
             * @throws detail::ExpectationFailed If the substring is not found.
             */
            void toMatch(const std::string& sub) const {
                static_assert(StringLike<T>, "toMatch needs a string");
                check(std::string_view(actual).find(sub) != std::string_view::npos, "toMatch(" + str(sub) + ")", str(actual));
            }
            void toMatch(const char* sub) const {
                toMatch(std::string(sub));
            }

            /**
             * @brief Checks whether a callable throws any exception.
             * @throws detail::ExpectationFailed If the callable does not throw.
             */
            void toThrow() const {
                bool threw = false;
                try {
                    actual();
                } catch (...) {
                    threw = true;
                }
                check(threw, "toThrow()", threw ? "threw" : "did not throw");
            }

            /**
             * @brief Checks whether a callable throws an exception whose message contains a substring
             * @param substr Substring expected in the exception message.
             * @throws detail::ExpectationFailed If the callable does not throw a matching exception.
             */
            void toThrow(const std::string& substr) const {
                bool ok = false;
                std::string what = "did not throw";
                try {
                    actual();
                } catch (const std::exception& e) {
                    what = std::string("threw \"") + e.what() + "\"";
                    ok = std::string_view(e.what()).find(substr) != std::string_view::npos;
                } catch (...) {
                    what = "threw a non-std exception";
                }
                check(ok, "toThrow(" + str(substr) + ")", what);
            }
        };

        /// @cond TURN_OFF_DOXYGEN

        template <class T>
        struct ValueHolder {
            T held;
            explicit ValueHolder(T&& v) : held(std::move(v)) {}
        };

        template <class T>
        struct Expectation : private ValueHolder<T>, public Matchers<T> {
            Matchers<T> never; // "not" is reserved keyword
            explicit Expectation(T v) : ValueHolder<T>(std::move(v)), Matchers<T>(this->held, false), never(this->held, true) {}
            Expectation(const Expectation&) = delete;
            Expectation& operator=(const Expectation&) = delete;
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

        // Runner class
        using Hooks = std::vector<std::function<void()>>;

        // non owning reference to a callable
        class FnRef {
        private:
            void* obj;
            void (*call)(void*);

        public:
            template <class F> requires (!std::is_same_v<std::remove_cvref_t<F>, FnRef>)
            FnRef(F&& f)
                : obj(const_cast<void*>(static_cast<const void*>(std::addressof(f)))),
                  call([](void* o) { (*static_cast<std::remove_reference_t<F>*>(o))(); }) {}
            void operator()() const { call(obj); }
        };

        struct Frame {
            std::string name;
            Hooks before_all, after_all, before_each, after_each;
            std::size_t beforeAllRan = 0; // beforeAll hooks run lazily right before the first test that actually runs
            bool testsRan = false; // afterAll only runs if some test ran in this block
        };

        class Runner {
        private:
            using Clock = std::chrono::steady_clock;
            Options opts;
            std::regex re;
            bool hasRe = false;
            bool initialized_ = false;
            std::ostream& out = std::cout;
            Frame root;
            std::vector<Frame*> chain{&root}; // root .. innermost describe
            std::size_t headingsPrinted = 1; // verbose: how many chain entries have had their heading printed
            int passed = 0, failed = 0, skipped = 0;
            Clock::time_point start = Clock::now();

            static constexpr const char *GREEN = "\033[32m",
                                        *RED = "\033[31m",
                                        *YELLOW = "\033[33m",
                                        *DIM = "\033[2m",
                                        *RESET = "\033[0m";

            std::string paint(const char* code, const std::string& s) const {
                return opts.color ? std::string(code) + s + RESET : s;
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
                out << std::flush;
            }

            void runHook(const std::function<void()>& hook, const std::string& label) {
                try {
                    hook();
                } catch (const std::exception& e) {
                    report(false, label, std::string("Unexpected exception: ") + e.what(), 0);
                } catch (...) {
                    report(false, label, "Unexpected non-std exception", 0);
                }
            }

        public:
            Runner() {
                if (const char* nc = std::getenv("NO_COLOR"); nc && *nc)
                    opts.color = false;
            }
            Runner(const Runner&) = delete;
            Runner& operator=(const Runner&) = delete;
            static Runner& get() { static Runner r; return r; }

            bool initialized() const {
                return initialized_;
            }

            void init(int argc, char** argv) {
                std::vector<std::string> args;
                for (int i = 1; i < argc; ++i) args.emplace_back(argv[i]);
                if (auto code = parseArgs(args, argc > 0 ? argv[0] : "tests", opts)) std::exit(*code);
                if (!opts.pattern.empty()) {
                    std::string error;
                    if (regexAssign(re, opts.pattern, error))
                        hasRe = true;
                    else {
                        std::cerr << "Invalid --testNamePattern regex: " << error << '\n';
                        std::exit(2);
                    }
                }
                initialized_ = true;
            }

            void enter(Frame* f) {
                chain.push_back(f);
            }
            void leave(Frame* f) {
                if (f->testsRan)
                    for (const auto& h : f->after_all) runHook(h, "afterAll");
                chain.pop_back();
                if (headingsPrinted > chain.size())
                    headingsPrinted = chain.size();
            }

            void addHook(Hooks Frame::*slot, std::function<void()> fn) {
                (chain.back()->*slot).push_back(std::move(fn));
            }

            void runTest(const std::string& name, const std::function<void()>& fn, int timeoutMs, bool expectFail) {
                std::string full;
                for (std::size_t i = 1; i < chain.size(); ++i)
                    full += chain[i]->name + " ";
                full += name;
                if (hasRe && !regexSearch(full, re)) {
                    ++skipped;
                    return;
                }

                for (Frame* f : chain) f->testsRan = true;
                for (Frame* f : chain)
                    while (f->beforeAllRan < f->before_all.size()) runHook(f->before_all[f->beforeAllRan++], "beforeAll");

                const int timeout = timeoutMs >= 0 ? timeoutMs : opts.timeoutMs;
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
                    for (const Frame* f : chain)
                        for (const auto& h : f->before_each) h();
                });

                if (!testFailed) {
                    const auto t0 = Clock::now();
                    guarded(fn);
                    ms = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
                    // tests are synchronous, so just check timeout at the end of it
                    if (!testFailed && ms > timeout)
                        fail("Exceeded timeout of " + std::to_string(timeout) + " ms for a test (took " + std::to_string(ms) + " ms).");
                }

                guarded([&] {
                    for (auto rit = chain.rbegin(); rit != chain.rend(); ++rit)
                        for (const auto& h : (*rit)->after_each) h();
                });

                if (expectFail) {
                    report(testFailed, name, testFailed ? "" : "Failing test passed even though it was supposed to fail.", ms);
                } else {
                    report(!testFailed, name, error, ms);
                }
            }

            int finish() {
                const double secs = std::chrono::duration<double>(Clock::now() - start).count();
                if (opts.verbose)
                    out << '\n'; // failure blocks in quiet mode already end with blank line

                out << "Tests:  "
                    << paint(RED, std::to_string(failed) + " failed") << ", "
                    << paint(GREEN, std::to_string(passed) + " passed")
                    << (skipped ? ", " + paint(YELLOW, std::to_string(skipped) + " skipped") : "")
                    << ", " << (passed + failed + skipped) << " total\n"
                    << "Time:   " << std::fixed << std::setprecision(3) << secs << " s\n";
                return failed ? 1 : 0;
            }
        };

        // entering a describe block pushes a frame, leaving it pops (and runs afterAll hooks).
        struct DescribeScope {
            Frame frame;
            explicit DescribeScope(const std::string& name) { frame.name = name; Runner::get().enter(&frame); }
            ~DescribeScope() { Runner::get().leave(&frame); }
            DescribeScope(const DescribeScope&) = delete;
            DescribeScope& operator=(const DescribeScope&) = delete;
        };

        // 'TestRegistrar{"name"} = [&]() { ... };' -> runs the test right now (used by the it/test macros)
        struct TestRegistrar {
            std::string name;
            int timeoutMs;
            explicit TestRegistrar(std::string n, int timeout = -1) : name(std::move(n)), timeoutMs(timeout) {}
            template <class F> requires std::is_invocable_v<F&>
            void operator=(F&& fn) const { Runner::get().runTest(name, FnRef(fn), timeoutMs, false); }
        };
        struct HookRegistrar {
            Hooks Frame::*slot;
            void operator=(std::function<void()> fn) const { Runner::get().addHook(slot, std::move(fn)); }
        };

        // .each/.failing
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
            else if constexpr (std::is_same_v<Row, char> && !std::is_invocable_v<F&, const Row&> && std::is_invocable_v<F&, std::string>)
                f(std::string(1, row)); // for character literals
            else f(row);
        }

        /// @endcond

        template <class Row>
        class EachBinder {
        private:
            std::vector<Row> rows;
            bool failing;

        public:
            EachBinder(std::vector<Row> r, bool fail) : rows(std::move(r)), failing(fail) {}
            template <class F>
            void operator()(const std::string& name, F&& fn, int timeoutMs = -1) const {
                for (std::size_t i = 0; i < rows.size(); ++i) {
                    Row row = rows[i];
                    Runner::get().runTest(formatTitle(name, rowArgs(row), i), [&] { callWithRow(fn, row); }, timeoutMs, failing);
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
                for (std::size_t i = 0; i < rows.size(); ++i) {
                    DescribeScope scope{formatTitle(name, rowArgs(rows[i]), i)};
                    callWithRow(fn, rows[i]);
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
            void operator()(const std::string& name, const std::function<void()>& fn, int timeoutMs = -1) const {
                Runner::get().runTest(name, FnRef(fn), timeoutMs, true);
            }
        };

        struct TestApi {
            EachFn<EachBinder> each{false};
            FailingApi failing{};
            void operator()(const std::string& name, const std::function<void()>& fn, int timeoutMs = -1) const {
                Runner::get().runTest(name, FnRef(fn), timeoutMs, false);
            }
        };

        struct DescribeApi {
            EachFn<DescribeEachBinder> each{false};
            void operator()(const std::string& name, const std::function<void()>& fn) const {
                DescribeScope scope{name};
                fn();
            }
        };
    }

    // public API
    /**
     * @brief Registers and executes a test case.
     *
     * Supports ordinary tests, parameterized tests through .each(), and tests that are expected to fail through .failing().
     */
    inline constexpr detail::TestApi it{}, test{};

    /**
     * @brief Defines a test suite or nested test group.
     *
     * Groups tests under a descriptive name. Suites can register lifecycle hooks and contain nested suites.
     *
     * Supports parameterized suite definitions through .each().
     */
    inline constexpr detail::DescribeApi describe{};

    /**
     * @brief Creates an expectation for a value.
     *
     * The returned object provides matchers such as toBe(), toEqual(), toContain(), and toThrow().
     *
     * String literals are converted to std;:string before being stored.
     *
     * @tparam T Type of the value being tested.
     * @param value Value to test
     * @return An expectation object
     *
     * @code
     * expect(42).toBe(42);
     * expect("hello").toContain("ell");
     * @endcode
     */
    template <class T>
    auto expect(T&& value) {
        if constexpr (std::is_convertible_v<T, const char*> && !std::is_same_v<std::decay_t<T>, std::nullptr_t>)
            return detail::Expectation<std::string>(std::string(value)); // string literals -> std::string
        else
            return detail::Expectation<std::decay_t<T>>(std::forward<T>(value));
    }

    /**
     * @brief Creates a typed table of rows for parmeterized tests.
     *
     * Each row is a tuple. When passed to it.each(), its elements are supplied as callback arguments.
     *
     * @tparam Ts Types of tghe columns.
     * @param rows Initializer list of rows.
     * @return A vector containing the supplied rows.
     *
     * @code
     * it.each(table<int, int, int>({
     *     {1, 1, 2}
     *     {1, 2, 3}
     * })("%d + %d = %d", [](int a, int b, int sum) {
     *     expect(a + b).toBe(sum);
     * });
     * @endcode
     */
    template <class... Ts>
    std::vector<std::tuple<Ts...>> table(std::initializer_list<std::tuple<Ts...>> rows) {
        return std::vector<std::tuple<Ts...>>(rows);
    }

    /**
     * @brief Initializes the test runner for command line arguments.
     *
     * Call this function at the beginning of main(), before registering any suites or tests.
     *
     * Supported options include:
     * - `-v`, `--verbose`: Print every test result.
     * - `-t`, `--testNamePattern <regex>`: Run tests matching a regular expression against their full names.
     * - `--timeout <ms>`: Set the default per-test timeout.
     * - `--no-color`: Disable colored output.
     * - `-h`, `--help`: Print usage information and exit.
     *
     * @param argc Argument count from main().
     * @param argv Argument vector from main().
     *
     * @note Invalid arguments cause the process to exit with status of 2.
     */
    inline void init(int argc, char** argv) {
        detail::Runner::get().init(argc, argv);
    }

    /**
     * @brief Prints the test summary and returns the test result.
     *
     * @return 0 if all tests passed, 1 if any test failed.
     *
     * @note If init() was not called, the runner uses default options and prints a warning.
     */
    inline int run() {
        auto& r = detail::Runner::get();
        if (!r.initialized())
            std::cerr << "jest-cpp: command line options were ignored (used defaults); call jest::init(argc, argv) before the first describe/it\n";
        return r.finish();
    }
}

/// @cond TURN_OFF_DOXYGEN
#ifdef JEST_CPP_IMPLEMENTATION
namespace jest {
    namespace detail {
        JEST_CPP_INLINE bool regexSearch(const std::string& text, const std::regex& re) {
            return std::regex_search(text, re);
        }

        JEST_CPP_INLINE bool regexAssign(std::regex& out, const std::string& pattern, std::string& error) {
            try {
                out = std::regex(pattern);
                return true;
            } catch (const std::regex_error& e) {
                error = e.what();
                return false;
            }
        }
    }

    JEST_CPP_INLINE Regex::Regex(std::string src, std::regex::flag_type f) : std::regex(src, f), source(std::move(src)) {}

    inline namespace literals {
        JEST_CPP_INLINE Regex operator""_re(const char* s, std::size_t n) {
            return Regex(std::string(s, n));
        }
    }
}
#endif
/// @endcond

// define JEST_NO_MACROS before including to not define the macros
#ifndef JEST_NO_MACROS

/// @cond TURN_OFF_DOXYGEN
#define JEST_CAT_(a, b) a##b
#define JEST_CAT(a, b) JEST_CAT_(a, b)
/// @endcond

/**
 * @def describe(name)
 * @brief Declares a name test suite.
 *
 * Creates a scope in which tests and nested suites can be registered.
 * Lifecycle hooks are associated with the current suite.
 *
 * @param name Descriptive name of the suite.
 *
 * @code
 * describe("math") {
 *     ...
 * }
 * @endcode
 */
#define describe(name) if (::jest::detail::DescribeScope JEST_CAT(jest_scope_, __COUNTER__){name}; true)

/**
 * @def it
 * @brief Declares a test case.
 *
 * Registers a test case with the current test suite.
 *
 * @par Supported syntax
 *
 * A test with default timeout:
 * @code
 * it("name") {
 *     ...
 * };
 * @endcode
 *
 * A test with a custom timeout:
 * @code
 * it("name", 5000) {
 *     ...
 * };
 * @endcode
 *
 *
 * @see test
 */
#define it(...) ::jest::detail::TestRegistrar{__VA_ARGS__} = [&]()

/**
 * @brief Alias for `it`
 * @see it
 */
#define test(...) it(__VA_ARGS__)

/**
 * @brief Code that is ran before all the test cases in a describe block
 *
 * Must be ran BEFORE any test case is called.
 *
 * @par Example
 * @code
 * beforeAll { counter = 0; };
 * @endcode
 */
#define beforeAll ::jest::detail::HookRegistrar{&::jest::detail::Frame::before_all} = [&]()

/**
 * @brief Code that is ran after every test case in a describe block
 *
 * Must be ran BEFORE any test case is called.
 * `afterAll` will run AFTER the local variables in the describe's block has been deallocated.
 *
 * @par Example
 * @code
 * afterAll { cleanup(); };
 * @endcode
 */
#define afterAll ::jest::detail::HookRegistrar{&::jest::detail::Frame::after_all} = [&]()

/**
 * @brief Code that is ran before each test case in a describe block
 *
 * Must be ran BEFORE any test case is called.
 *
 * @par Example
 * @code
 * beforeEach { counter = 0; };
 * @endcode
 */
#define beforeEach ::jest::detail::HookRegistrar{&::jest::detail::Frame::before_each} = [&]()

/**
 * @brief Code that is ran after each test case in a describe block
 *
 * Must be ran BEFORE any test case is called.
 *
 * @par Example
 * @code
 * afterEach { counter = 0; };
 * @endcode
 */
#define afterEach ::jest::detail::HookRegistrar{&::jest::detail::Frame::after_each} = [&]()

#endif