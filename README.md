## UT: run-time and compile-time C++ unit-testing

> [!IMPORTANT]
>
> **This is a simplification of [boost-ext ut2](https://github.com/boost-ext/ut2) with some added functionality from [boost-ext ut](https://github.com/boost-ext/ut).**
>
> This library is only for those who want an extremely stripped down and basic unit test library.

## Tools

- `suite` - Write a collection of tests
- `"name"_test` - Declare a test
- `test("name")` - Declare a test with a runtime name
- `expect` - Check a boolean
- `throws(func)` - Require a call to throw

### Features

- Single header

### Requirements

- C++20

## Example

```c++
#include "ut/ut.hpp"

using namespace ut;

suite tests = [] {
   "double"_test = [] {
      double v = 42.1;
      expect(42.1 == v) << "v is not 42.1";
      expect[42.1 == v] << "a fatal error!";
   };

   "double mutable"_test = []() mutable {
      double v = 42.1;
      expect(42.1 == v) << "v is not 42.1";
      expect[42.1 == v] << "oh no!";
   };

   "int"_test = [] {
      expect(5 + 4 == 9) << "bad";
      expect[5 + 4 == 9] << "fatal";
   };

   "int consteval"_test = []() consteval {
      expect(5 + 4 == 9) << "bad";
      expect[5 + 4 == 9] << "fatal";
   };

   "string"_test = [] {
      std::string_view v = "Hello World";
      expect(v == "Hello World");
      expect[v == "Hello World"];
   };

   test("runtime named test") = [] {
      std::string_view v = "Hello World";
      expect(v == "Hello World");
      expect[v == "Hello World"];
   };

   "throws"_test = []() mutable { expect(throws([] { throw std::runtime_error("I throw!"); })); };

   "no throw"_test = []() mutable { expect(not throws([] { return 55; })); };
};

int main() {}
```

## Q&A

- Why call this ut and use the same namespace? The purpose is for this library to be a subset of ut2 and allow swapping between the libraries.
