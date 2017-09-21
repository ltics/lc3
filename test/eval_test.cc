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

TEST_CASE("test eval let statements") {
  struct TestCase {
    string input;
    int expected;
  };

  vector<TestCase> tests = {
    { "let a = 5; a;", 5 },
    { "let a = 5 * 5; a;", 25 },
    { "let a = 5; let b = a; b;", 5 },
    { "let a = 5; let b = a; let c = a + b + 5; c;", 15 }
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      auto evaluated = test_eval(c.input);
      test_integer_object(evaluated, c.expected);
    });
}

TEST_CASE("test eval function object") {
  auto input = "fn(x) { x + 2; };";

  auto evaluated = test_eval(input);
  REQUIRE(evaluated->type() == FUNCTION_OBJ);

  auto func = static_pointer_cast<Function>(evaluated);

  REQUIRE(func->parameters.size() == 1);
  REQUIRE(func->parameters[0]->to_string() == "x");
  REQUIRE(func->body->to_string() == "(x + 2)");
}

TEST_CASE("test function application") {
  struct TestCase {
    string input;
    int expected;
  };

  vector<TestCase> tests = {
    { "let identity = fn(x) { x; }; identity(5);", 5 },
    { "let identity = fn(x) { return x; }; identity(5);", 5 },
    { "let double = fn(x) { x * 2; }; double(5);", 10 },
    { "let add = fn(x, y) { x + y; }; add(5, 5);", 10 },
    { "let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20 },
    { "fn(x) { x; }(5)", 5 }
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      test_integer_object(test_eval(c.input), c.expected);
    });
}

TEST_CASE("test enclosing environments") {
  auto input = "\
    let first = 10; \
    let second = 10; \
    let third = 10; \
    \
    let ourFunction = fn(first) { \
      let second = 20; \
      \
      first + second + third; \
    };\
    \
    ourFunction(20) + first + second;";

  test_integer_object(test_eval(input), 70);
}

TEST_CASE("test closures") {
  auto input = "\
    let newAdder = fn(x) { \
      fn(y) { x + y }; \
    }; \
    \
    let addTwo = newAdder(2); \
    addTwo(2);";

  test_integer_object(test_eval(input), 4);
}

TEST_CASE("test string literal") {
  auto input = "\"Hello Cleantha\"";
  auto evaluated = test_eval(input);

  REQUIRE(evaluated->type() == STRING_OBJ);
  REQUIRE(static_pointer_cast<String>(evaluated)->value == "Hello Cleantha");
}

TEST_CASE("test string concatenation") {
  auto input = "\"Hello\" + \" \" + \"Cleantha!\"";
  auto evaluated = test_eval(input);

  REQUIRE(evaluated->type() == STRING_OBJ);
  REQUIRE(static_pointer_cast<String>(evaluated)->value == "Hello Cleantha!");
}

TEST_CASE("test error handling") {
  struct TestCase {
    string input;
    string expected;
  };

  vector<TestCase> tests = {
    {
      "5 + true;",
      "type mismatch: INTEGER + BOOLEAN"
    },
    {
      "5 + true; 5;",
      "type mismatch: INTEGER + BOOLEAN"
    },
    {
      "-true",
      "unknown operator: -BOOLEAN"
    },
    {
      "true + false;",
      "unknown operator: BOOLEAN + BOOLEAN"
    },
    {
      "true + false + true + false;",
      "unknown operator: BOOLEAN + BOOLEAN"
    },
    {
      "5; true + false; 5",
      "unknown operator: BOOLEAN + BOOLEAN"
    },
    {
      "\"Hello\" - \"World\"",
      "unknown operator: STRING - STRING"
    },
    {
      "if (10 > 1) { true + false; }",
      "unknown operator: BOOLEAN + BOOLEAN"
    },
    {
      "if (10 > 1) { \
         if (10 > 1) { \
           return true + false; \
         } \
         \
         return 1; \
       }",
      "unknown operator: BOOLEAN + BOOLEAN"
    },
    {
      "foobar",
      "identifier not found: foobar"
    },
    {
      "{\"name\": \"Monkey\"}[fn(x) { x }];",
      "unusable as hash key: FUNCTION"
    },
    {
      "999[1]",
      "index operator not supported: INTEGER"
    }
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      auto evaluated = test_eval(c.input);

      REQUIRE(evaluated->type() == ERROR_OBJ);
      REQUIRE(static_pointer_cast<Error>(evaluated)->message == c.expected);
    });
};

TEST_CASE("test builtin functions") {
  struct IntTestCase {
    string input;
    int expected;
  };

  struct ArrayTestCase {
    string input;
    vector<int> expected;
  };

  struct StringTestCase {
    string input;
    string expected;
  };

  vector<IntTestCase> int_tests = {
    { "len(\"\")", 0 },
    { "len(\"four\")", 4 },
    { "len(\"hello world\")", 11 },
    { "len([1, 2, 3])", 3 },
    { "len([])", 0 },
    { "first([1, 2, 3])", 1 },
    { "last([1, 2, 3])", 3 }
  };

  vector<StringTestCase> err_tests = {
    { "len(1)", "argument to `len` not supported, got INTEGER" },
    { "len(\"one\", \"two\")", "wrong number of arguments. got=2, want=1" },
    { "first(1)", "argument to `first` must be ARRAY, got INTEGER" },
    { "last(1)", "argument to `last` must be ARRAY, got INTEGER" },
    { "push(1, 1)", "argument to `push` must be ARRAY, got INTEGER" }
  };

  vector<ArrayTestCase> arr_tests = {
    { "rest([1, 2, 3])", vector<int>({ 2, 3 }) },
    { "push([], 1)", vector<int>({ 1 }) }
  };

  vector<string> null_tests = {
    "puts(\"hello\", \"world!\")",
    "first([])",
    "last([])",
    "rest([])"
  };

  std::for_each(int_tests.cbegin(), int_tests.cend(), [](IntTestCase c) {
      REQUIRE(static_pointer_cast<Integer>(test_eval(c.input))->value == c.expected);
    });

  std::for_each(err_tests.cbegin(), err_tests.cend(), [](StringTestCase c) {
      REQUIRE(static_pointer_cast<Error>(test_eval(c.input))->message == c.expected);
    });

  std::for_each(arr_tests.cbegin(), arr_tests.cend(), [](ArrayTestCase c) {
      vector<int> int_arr = static_pointer_cast<Array>(test_eval(c.input))->elements | view::transform([](shared_ptr<Object> obj) {
          return static_pointer_cast<Integer>(obj)->value;
        });
      REQUIRE(int_arr == c.expected);
    });

  std::for_each(null_tests.cbegin(), null_tests.cend(), [](string input) {
      test_null_object(test_eval(input));
    });
}

TEST_CASE("test array literals") {
  auto input = "[1, 2 * 2, 3 + 3]";
  auto evaluated = test_eval(input);

  REQUIRE(evaluated->type() == ARRAY_OBJ);
  auto arr = static_pointer_cast<Array>(evaluated);

  REQUIRE(arr->elements.size() == 3);
  vector<int> int_arr = arr->elements | view::transform([](shared_ptr<Object> o) { return static_pointer_cast<Integer>(o)->value; });
  vector<int> expected = { 1, 4, 6 };
  REQUIRE(int_arr == expected);
}

TEST_CASE("test array index expressions") {
  struct TestCase {
    string input;
    int expected;
  };

  vector<TestCase> tests = {
    {
      "[1, 2, 3][0]",
      1
    },
    {
      "[1, 2, 3][1]",
      2
    },
    {
      "[1, 2, 3][2]",
      3
    },
    {
      "let i = 0; [1][i];",
      1
    },
    {
      "[1, 2, 3][1 + 1];",
      3
    },
    {
      "let myArray = [1, 2, 3]; myArray[2];",
      3
    },
    {
      "let myArray = [1, 2, 3]; myArray[0] + myArray[1] + myArray[2];",
      6
    },
    {
      "let myArray = [1, 2, 3]; let i = myArray[0]; myArray[i]",
      2
    }
  };

  vector<string> null_tests = {
    "[1, 2, 3][3]",
    "[1, 2, 3][-1]"
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      test_integer_object(test_eval(c.input), c.expected);
    });

  std::for_each(null_tests.cbegin(), null_tests.cend(), [](string input) {
      test_null_object(test_eval(input));
    });
}

TEST_CASE("test hash literals") {
  auto input = "\
    let two = \"two\"; \
    { \
      \"one\": 10 - 9, \
      two: 1 + 1, \
      \"thr\" + \"ee\": 6 / 2, \
      4: 4, \
      true: 5, \
      false: 6 \
    }";

  auto evaluated = test_eval(input);
  REQUIRE(evaluated->type() == HASH_OBJ);
  auto hash = static_pointer_cast<Hash>(evaluated);

  map<string, int> expected = {
    { make_shared<String>("one")->hash_key(), 1 },
    { make_shared<String>("two")->hash_key(), 2 },
    { make_shared<String>("three")->hash_key(), 3 },
    { make_shared<Integer>(4)->hash_key(), 4 },
    { make_shared<object::Boolean>(true)->hash_key(), 5 },
    { make_shared<object::Boolean>(false)->hash_key(), 6 }
  };

  std::for_each(expected.cbegin(), expected.cend(), [&](pair<string, int> p) {
      test_integer_object(hash->pairs[p.first].second, p.second);
    });
}

TEST_CASE("test hash index expressions") {
  struct TestCase {
    string input;
    int expected;
  };

  vector<TestCase> tests = {
    {
      "{\"foo\": 5}[\"foo\"]",
      5
    },
    {
      "let key = \"foo\"; {\"foo\": 5}[key]",
      5
    },
    {
      "{5: 5}[5]",
      5
    },
    {
      "{true: 5}[true]",
      5
    },
    {
      "{false: 5}[false]",
      5
    }
  };

  vector<string> null_tests = {
    "{\"foo\": 5}[\"bar\"]",
    "{}[\"foo\"]"
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      test_integer_object(test_eval(c.input), c.expected);
    });

  std::for_each(null_tests.cbegin(), null_tests.cend(), [](string input) {
      test_null_object(test_eval(input));
    });
}
