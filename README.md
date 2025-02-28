## UT: run-time and compile-time C++ unit-testing

A simple and fast compiling unit test library.

## Tools

- `suite` - Write a collection of tests
- `"name"_test` - Declare a test
- `test("name")` - Declare a test with a runtime name
- `expect` - Check a boolean
- `throws(func)` - Require a call to throw

### Features

- Single header

To enable compile time testing you must define the macro: `UT_COMPILE_TIME`

Runtime testing is always enabled.

### Requirements

- C++23

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
