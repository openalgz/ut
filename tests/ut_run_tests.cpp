// Tests for UT_RUN environment variable filtering feature
// This file is used by CTest to verify the UT_RUN functionality

#include "ut/ut.hpp"

using namespace ut;

// Define exactly 5 tests with known names for predictable testing
suite filter_tests = [] {
   "alpha"_test = [] {
      expect(true);
   };

   "beta"_test = [] {
      expect(true);
   };

   "gamma"_test = [] {
      expect(true);
   };

   test("delta test") = [] {
      expect(true);
   };

   test("epsilon test") = [] {
      expect(true);
   };
};

int main() {}
