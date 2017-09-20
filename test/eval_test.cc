#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "../src/lexer.hpp"
#include "../src/parser.hpp"
#include "../src/object.hpp"
#include "../src/eval.hpp"
#include "./util.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <functional>

using namespace std;
using namespace lexer;
using namespace parser;
using namespace object;
using namespace eval;
using namespace testutil;

auto test_eval(string input) -> shared_ptr<Object> {
  auto lexer = Lexer::new_lexer(input);
  auto parser = Parser::new_parser(lexer);
  auto program = parser->parse_program();
  auto env = make_shared<Environment>();

  return eval::eval(program, env);
}

auto test_integer_object(shared_ptr<Object> obj, int expected) -> void {
  REQUIRE(obj->type() == INTEGER_OBJ);
  REQUIRE(static_pointer_cast<Integer>(obj)->value == expected);
}

auto test_boolean_object(shared_ptr<Object> obj, int expected) -> void {
  REQUIRE(obj->type() == BOOLEAN_OBJ);
  REQUIRE(static_pointer_cast<object::Boolean>(obj)->value == expected);
}

auto test_null_object(shared_ptr<Object> obj) -> void {
  REQUIRE(obj->type() == NULL_OBJ);
}

TEST_CASE("test eval integer expression") {
  struct TestCase {
    string input;
    int expected;
  };

  vector<TestCase> tests = {
    { "5", 5 },
    { "10", 10 },
    { "-5", -5 },
    { "-10", -10 },
    { "5 + 5 + 5 + 5 - 10", 10 },
    { "2 * 2 * 2 * 2 * 2", 32 },
    { "-50 + 100 + -50", 0 },
    { "5 * 2 + 10", 20 },
    { "5 + 2 * 10", 25 },
    { "20 + 2 * -10", 0 },
    { "50 / 2 * 2 + 10", 60 },
    { "2 * (5 + 10)", 30 },
    { "3 * 3 * 3 + 10", 37 },
    { "3 * (3 * 3) + 10", 37 },
    { "(5 + 10 * 2 + 15 / 3) * 2 + -10", 50 }
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      auto evaluated = test_eval(c.input);
      test_integer_object(evaluated, c.expected);
    });
}

TEST_CASE("test eval boolean expression") {
  struct TestCase {
    string input;
    bool expected;
  };

  vector<TestCase> tests = {
    { "true", true },
    { "false", false },
    { "1 < 2", true },
    { "1 > 2", false },
    { "1 < 1", false },
    { "1 > 1", false },
    { "1 == 1", true },
    { "1 != 1", false },
    { "1 == 2", false },
    { "1 != 2", true },
    { "true == true", true },
    { "false == false", true },
    { "true == false", false },
    { "true != false", true },
    { "false != true", true },
    { "(1 < 2) == true", true },
    { "(1 < 2) == false", false },
    { "(1 > 2) == true", false },
    { "(1 > 2) == false", true }
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      auto evaluated = test_eval(c.input);
      test_boolean_object(evaluated, c.expected);
    });
}

TEST_CASE("test eval bang operator") {
  struct TestCase {
    string input;
    bool expected;
  };

  vector<TestCase> tests = {
    {"!true", false},
    {"!false", true},
    {"!5", false},
    {"!!true", true},
    {"!!false", false},
    {"!!5", true}
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      auto evaluated = test_eval(c.input);
      test_boolean_object(evaluated, c.expected);
    });
}

TEST_CASE("test eval if expression") {
  struct TestCase {
    string input;
    int expected;
  };

  vector<TestCase> tests = {
    {"if (true) { 10 }", 10},
    {"if (false) { 10 }", 0},
    {"if (1) { 10 }", 10},
    {"if (1 < 2) { 10 }", 10},
    {"if (1 > 2) { 10 }", 0},
    {"if (1 > 2) { 10 } else { 20 }", 20},
    {"if (1 < 2) { 10 } else { 20 }", 10}
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      auto evaluated = test_eval(c.input);
      if (c.expected > 0) {
        test_integer_object(evaluated, c.expected);
      } else {
        test_null_object(evaluated);
      }
    });
}

TEST_CASE("test eval return statements") {
  struct TestCase {
    string input;
    int expected;
  };

  vector<TestCase> tests = {
    { "return 10;", 10},
    { "return 10; 9;", 10},
    { "return 2 * 5; 9;", 10},
    { "9; return 2 * 5; 9;", 10},
    { "if (10 > 1) { return 10; }", 10},
    { "if (10 > 1) { \
         if (10 > 1) { \
           return 10; \
         } \
         return 1; \
       }", 10 },
    { "let f = fn(x) { \
         return x; \
         x + 10; \
       }; \
       f(10);", 10 },
    { "let f = fn(x) { \
         let result = x + 10; \
         return result; \
         return 10; \
       }; \
       f(10);", 20 }
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      auto evaluated = test_eval(c.input);
      test_integer_object(evaluated, c.expected);
    });
}
