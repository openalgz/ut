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

## Q&A

- Why call this ut and use the same namespace? The purpose is for this library to be a subset of ut2 and allow swapping between the libraries.
