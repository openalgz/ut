## UT: run-time and compile-time C++ unit-testing

A simple and fast compiling unit test library.

## Tools

- `suite` - Write a collection of tests
- `"name"_test` - Declare a test
- `test("name")` - Declare a test with a runtime name
- `expect` - Check a boolean
- `throws(func)` - Require a call to throw

### Features

#### Single header

To enable compile time testing, set the option `UT_COMPILE_TIME` to ON. For example:
```cmake
set(UT_COMPILE_TIME ON)
```

Runtime testing is always enabled.

#### Modules support

The library supports C++ 20/23 modules. To enable, set the option `UT_ENABLE_MODULES` to ON. For example:
```cmake
set (UT_ENABLE_MODULES ON)
```
You can then import the library:
```cpp
import ut;
```

Please note that modules integration requires compiler and build tools that support `import std`.

##### Consuming ut as a subproject

CMake puts `import std` behind an experimental UUID that changes between CMake releases. When ut is the top-level project it selects the right value for the running CMake on its own, and nothing else is needed.

That is not possible when ut is added with `add_subdirectory()` or `FetchContent`. CMake reads the value while enabling `CXX`, which your own `project()` call has already done before ut is added, so ut is reached too late to set it. Set it yourself, before `project()`:

```cmake
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d")
project(my_project LANGUAGES CXX)
```

or pass it on the command line, which works the same way:

```bash
cmake -B build -DCMAKE_EXPERIMENTAL_CXX_IMPORT_STD=f35a9ac6-8463-4d38-8eec-5d6008153e7d
```

Otherwise CMake fails with `Experimental 'import std' support not enabled when detecting toolchain`.

The value above is the one for CMake 4.4. Each release publishes its own in CMake's `Help/dev/experimental.rst`, and the table at the top of ut's `CMakeLists.txt` lists those known to work.

### Running Specific Tests

Use the `UT_RUN` environment variable to run specific tests by name:

```bash
# Single test
UT_RUN="test name" ./my_tests

# Multiple tests
UT_RUN="[test1,test2,test3]" ./my_tests
```

### Requirements

- C++23
- CMake 3.31

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
