// Refactored from:
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstdlib>
#include <cstdint>
#include <concepts>
#include <source_location>

namespace ut
{
   namespace detail
   {
      template <class>
      constexpr auto is_mutable_lambda_v = false;
      template <class R, class B, class... Ts>
      constexpr auto is_mutable_lambda_v<R (B::*)(Ts...)> = true;
      template <class Fn>
      constexpr auto has_capture_lambda_v = sizeof(Fn) > 1ul;
      
      template <class T, class...>
      struct identity
      {
         using type = T;
      };
      template <size_t Size>
      struct fixed_string
      {
         constexpr fixed_string(const char (&str)[Size])
         {
            for (size_t i = 0; i < Size; ++i) {
               storage[i] = str[i];
            }
         }
         [[nodiscard]] constexpr auto operator[](const auto i) const { return storage[i]; }
         [[nodiscard]] constexpr auto data() const { return storage; }
         [[nodiscard]] static constexpr auto size() { return Size; }
         constexpr friend auto operator<<(auto& os, const fixed_string& fs) -> decltype(auto)
         {
            return os << fs.storage;
         }
         char storage[Size]{};
      };
   }
   namespace events
   {
      enum class mode { run_time, compile_time };
      template <mode Mode>
      struct test_begin
      {
         const char* file_name{};
         int line{};
         const char* name{};
      };
      template <mode Mode>
      struct test_end
      {
         const char* file_name{};
         int line{};
         const char* name{};
         enum { FAILED, PASSED, COMPILE_TIME } result{};
      };
      template <class Expr>
      struct assert_pass
      {
         const char* file_name{};
         int line{};
         Expr expr{};
      };
      template <class Expr>
      struct assert_fail
      {
         const char* file_name{};
         int line{};
         Expr expr{};
      };
      struct fatal
      {};
      template <class Msg>
      struct log
      {
         const Msg& msg;
         bool result{};
      };
      struct summary
      {
         enum { FAILED, PASSED, COMPILE_TIME };
         size_t asserts[2]{}; /* FAILED, PASSED */
         size_t tests[3]{}; /* FAILED, PASSED, COMPILE_TIME */
      };
   } // namespace events

   template <class OStream>
   struct outputter
   {
      template <events::mode Mode>
      constexpr auto on(const events::test_begin<Mode>&)
      {}
      constexpr auto on(const events::test_begin<events::mode::run_time>& event) { current_test = event; }
      template <events::mode Mode>
      constexpr auto on(const events::test_end<Mode>&)
      {}
      template <class Expr>
      constexpr auto on(const events::assert_pass<Expr>&)
      {}
      template <class Expr>
      constexpr auto on(const events::assert_fail<Expr>& event)
      {
         if (!std::is_constant_evaluated()) {
            if (initial_new_line == '\n') {
               os << initial_new_line;
            }
            else {
               initial_new_line = '\n';
            }
            os << event.file_name << ':' << event.line << ':' << "FAILED:" << '\"' << current_test.name
               << "\": " << event.expr;
         }
      }
      constexpr auto on(const events::fatal&) {}
      template <class Msg>
      constexpr auto on(const events::log<Msg>& event)
      {
         if (!std::is_constant_evaluated() && !event.result) {
            os << ' ' << event.msg;
         }
      }
      constexpr auto on(const events::summary& event)
      {
         using namespace events;
         if (!std::is_constant_evaluated()) {
            if (event.asserts[summary::FAILED] || event.tests[summary::FAILED]) {
               os << "\nFAILED: ";
            }
            else {
               os << "PASSED: ";
            }
            os << "tests: " << (event.tests[summary::PASSED] + event.tests[summary::FAILED]) << " ("
               << event.tests[summary::PASSED] << " passed, " << event.tests[summary::FAILED]
               << " failed, " << event.tests[summary::COMPILE_TIME] << " compile-time), "
               << "asserts: " << (event.asserts[summary::PASSED] + event.asserts[summary::FAILED])
               << " (" << event.asserts[summary::PASSED] << " passed, "
               << event.asserts[summary::FAILED] << " failed)\n";
         }
      }

      OStream& os;
      events::test_begin<events::mode::run_time> current_test{};
      char initial_new_line{};
   };

   template <class Outputter, uint32_t MaxDepth = 16>
   struct reporter
   {
      constexpr auto on(const events::test_begin<events::mode::run_time>& event)
      {
         asserts_failed[current++] = summary.asserts[events::summary::FAILED];
         outputter.on(event);
      }
      constexpr auto on(const events::test_end<events::mode::run_time>& event)
      {
         const auto result = summary.asserts[events::summary::FAILED] == asserts_failed[--current];
         ++summary.tests[result];
         events::test_end<events::mode::run_time> te{event};
         te.result = static_cast<decltype(te.result)>(result);
         outputter.on(te);
      }
      constexpr auto on(const events::test_begin<events::mode::compile_time>&)
      {
         ++summary.tests[events::summary::COMPILE_TIME];
      }
      constexpr auto on(const events::test_end<events::mode::compile_time>&) {}
      template <class Expr>
      constexpr auto on(const events::assert_pass<Expr>& event)
      {
         ++summary.asserts[events::summary::PASSED];
         outputter.on(event);
      }
      template <class Expr>
      constexpr auto on(const events::assert_fail<Expr>& event)
      {
         ++summary.asserts[events::summary::FAILED];
         outputter.on(event);
      }
      constexpr auto on(const events::fatal& event)
      {
         ++summary.tests[events::summary::FAILED];
         outputter.on(event);
         outputter.on(summary);
         std::abort();
      }

#if !defined(UT_COMPILE_TIME_ONLY)
      ~reporter()
      { // non constexpr
         outputter.on(summary);
         if (summary.asserts[events::summary::FAILED]) {
            std::abort();
         }
      }
#endif

      Outputter& outputter;
      events::summary summary{};
      size_t asserts_failed[MaxDepth]{};
      size_t current{};
   };

   template <class Reporter>
   struct runner
   {
      template <class Test>
      constexpr auto on(Test test, const char* file_name, int line, const char* name) -> bool
      {
         if (std::is_constant_evaluated()) {
            if constexpr (!detail::is_mutable_lambda_v<decltype(&Test::operator())>) {
               test();
               return true;
            }
            else {
               return false;
            }
         }
         else {
#if !defined(UT_RUN_TIME_ONLY)
            if constexpr (!detail::is_mutable_lambda_v<decltype(&Test::operator())> &&
                          !detail::has_capture_lambda_v<Test>) {
               reporter.on(events::test_begin<events::mode::compile_time>{file_name, line, name});
               static_assert((test(), "[FAILED]"));
               reporter.on(events::test_end<events::mode::compile_time>{file_name, line, name});
            }
#endif

#if !defined(UT_COMPILE_TIME_ONLY)
            reporter.on(events::test_begin<events::mode::run_time>{file_name, line, name});
            test();
            reporter.on(events::test_end<events::mode::run_time>{file_name, line, name});
#endif
         }
         return true;
      }

      Reporter& reporter;
   };
}

namespace ut
{
   template <class...>
   struct default_cfg
   {
      struct
      {
         friend constexpr decltype(auto) operator<<([[maybe_unused]] auto& os, [[maybe_unused]] const auto& t)
         {
#if defined(UT_COMPILE_TIME_ONLY)
            return os;
#else
            static_assert(
               requires { std::clog << t; },
               "[ERROR] No output supported: Consider #include <iostream> | ut::cfg<ut::override> = custom_cfg{} | "
               "#define UT_COMPILE_TIME_ONLY");
            return (std::clog << t);
#endif
         }
      } stream;
      ut::outputter<decltype(stream)> outputter{stream};
      ut::reporter<decltype(outputter)> reporter{outputter};
      ut::runner<decltype(reporter)> runner{reporter};
   };

   struct override
   {};

   /**
    * Customization point to override the default configuration
    *
    * @code
    * template<class... Ts> auto ut::cfg<ut::override, Ts...> = my_config{};
    * @endcode
    */
   template <class... Ts>
   inline default_cfg<Ts...> cfg{};

   namespace detail
   {
      template <class... Ts>
      [[nodiscard]] constexpr auto& cfg(Ts&&...)
      {
         return ut::cfg<typename detail::identity<override, Ts...>::type>;
      }
      void failed(); /// fail in constexpr context
   }

   constexpr struct
   {
      template <class Expr>
         requires (std::same_as<std::decay_t<Expr>, bool> || requires { static_cast<bool>(std::declval<Expr>()); })
      constexpr auto operator()(Expr expr, const char* file_name = __builtin_FILE(), int line = __builtin_LINE()) const
      {
         bool result{};
         if (result = static_cast<bool>(expr); std::is_constant_evaluated()) {
            if (!result) {
               detail::failed();
            }
         }
         else if (result) {
            detail::cfg(expr).reporter.on(events::assert_pass<Expr>{file_name, line, expr});
         }
         else {
            detail::cfg(expr).reporter.on(events::assert_fail<Expr>{file_name, line, expr});
         }
         return log{result};
      }

      struct fatal_expr
      {
         template <class Expr>
            requires (std::same_as<std::decay_t<Expr>, bool> || requires { static_cast<bool>(std::declval<Expr>()); })
         constexpr fatal_expr(Expr expr, const char* file_name = __builtin_FILE(), int line = __builtin_LINE())
         {
            bool result{};
            if (result = static_cast<bool>(expr); std::is_constant_evaluated()) {
               if (!result) {
                  detail::failed();
               }
            }
            else if (result) {
               detail::cfg(expr).reporter.on(events::assert_pass<Expr>{file_name, line, expr});
            }
            else {
               detail::cfg(expr).reporter.on(events::assert_fail<Expr>{file_name, line, expr});
               detail::cfg(expr).reporter.on(events::fatal{});
            }
         }
         bool result{};
      };
      constexpr auto operator[](fatal_expr e) const
      {
         return log{e.result};
      } /// multiple and/or default parameters requires C++23

     private:
      struct log
      {
         template <class Msg>
         constexpr const auto& operator<<(const Msg& msg) const
         {
            detail::cfg(msg).outputter.on(events::log<Msg>{msg, result});
            return *this;
         }
         bool result{};
      };
   } expect{};

   struct suite final
   {
      template <class Tests>
      suite(Tests tests)
      {
         tests();
      } // not constexpr
   };

   namespace detail
   {
      template <detail::fixed_string Name>
      struct test
      {
         struct run
         {
            template <class T>
            constexpr run(T test, const char* file_name = __builtin_FILE(), int line = __builtin_LINE())
               : result{cfg(test).runner.on(test, file_name, line, Name.data())}
            {}
            bool result{};
         };
         constexpr auto operator=(run test) const { return test.result; }
      };
   }

   template <detail::fixed_string Str>
   [[nodiscard]] constexpr auto operator""_test()
   {
      return detail::test<Str>{};
   }
}

using ut::operator""_test;
