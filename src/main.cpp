#ifdef UT_MODULE
import ut;
import std;
#else
#include "ut/ut.hpp"
#include <string_view>
#endif

using namespace ut;

suite tests = [] {
   "double"_test = [] {
      double v = 42.1;
      expect(42.1 == v) << "v is not 42.1";
      expect[42.1 == v] << "a fatal error!";
   };

   // a runtime-only test because the lambda is mutable
   "double mutable"_test = []() mutable {
      double v = 42.1;
      expect(42.1 == v) << "v is not 42.1";
      expect[42.1 == v] << "oh no!";
   };

   "int"_test = [] {
      expect(5 + 4 == 9) << "bad";
      expect[5 + 4 == 9] << "fatal";
   };

   // a compile-time only test because the lambda is consteval
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

struct boolean_type
{
   bool b = true;

   operator bool() const { return b; }
};

suite bool_convertible = [] { "boolean_type"_test = []() mutable { expect(boolean_type{}); }; };

int main() {
    return 0;
}
