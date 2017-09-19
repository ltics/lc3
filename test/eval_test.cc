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
