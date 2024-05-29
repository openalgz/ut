## UT: run-time and compile-time C++ unit-testing

> [!IMPORTANT]
>
> **This is a simplification of [boost-ext ut2](https://github.com/boost-ext/ut2). Go use that library.**
>
> This library is only for those who want an extremely stripped down and basic unit test library.

```c++
#include "ut/ut.hpp"
using namespace ut;
suite tests = [] {
   "double"_test = [] {
      double v = 42.1;
      expect(42.1 == v) << "v is not 42.1";
      expect[42.1 == v] << "a fatal error!";
   };
};
int main() {}
```

## Three Tools

1. `suite` - Write a collection of tests
2. `_test` - Declare a test
3. `expect` - Check a boolean

### Features

- Single header

### Requirements

- C++20

## Q&A

- Why call this ut and use the same namespace? The purpose is for this library to be a subset of ut2 and allow swapping between the libraries.
