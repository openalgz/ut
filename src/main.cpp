#include "ut/ut.hpp"

using namespace ut;

#include <exception>
#include <string_view>

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
