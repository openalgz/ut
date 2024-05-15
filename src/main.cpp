#include "ut/ut.hpp"

using namespace ut;

suite tests = [] {
   "my_test"_test = []() mutable {
      double x = 42;
      ut::expect(42.1 == x) << "x is not 42.1";
      ut::expect[42.1 == x];
   };
};

int main()
{
   
}
