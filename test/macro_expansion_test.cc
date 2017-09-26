#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "../src/ast.hpp"
#include "../src/lexer.hpp"
#include "../src/object.hpp"
#include "../src/parser.hpp"
#include "../src/eval.hpp"
#include "../src/macro_expansion.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <memory>

using namespace std;
using namespace ast;
using namespace lexer;
using namespace object;
using namespace parser;
using namespace eval;
using namespace macroexpansion;

auto test_parse_program(string input) -> shared_ptr<Program> {
  auto l = Lexer::new_lexer(input);
  auto p = Parser::new_parser(l);
  return p->parse_program();
}

TEST_CASE("test define macros") {
  auto input = " \
    let number = 1; \
    let function = fn(x, y) { x + y }; \
    let mymacro = macro(x, y) { x + y; }; \
    let mymacroTwo = macro(x, y) { x + y; }; \
    ";

  auto env = make_shared<Environment>();
  auto program = test_parse_program(input);

  define_macros(program, env);

  REQUIRE(program->statements.size() == 2); // it will filter out macro definition nodes and insert macro definition into the env
  REQUIRE(env->get("number") == nullptr);
  REQUIRE(env->get("function") == nullptr);
  REQUIRE(env->get("mymacro") != nullptr);
  REQUIRE(env->get("mymacroTwo") != nullptr);

  vector<shared_ptr<Object>> macros = { env->get("mymacro"), env->get("mymacroTwo") };

  std::for_each(macros.cbegin(), macros.cend(), [](shared_ptr<Object> obj) {
      REQUIRE(obj->type() == MACRO_OBJ);
      auto macro = static_pointer_cast<Macro>(obj);
      REQUIRE(macro->parameters.size() == 2);
      REQUIRE(macro->parameters[0]->to_string() == "x");
      REQUIRE(macro->parameters[1]->to_string() == "y");

      auto expected_body = "(x + y)";
      REQUIRE(macro->body->to_string() == expected_body);
    });
}

TEST_CASE("test expand macros") {
  struct TestCase {
    string input;
    string expected;
  };

  vector<TestCase> tests = {
    { "\
      let infixExpression = macro() { quote(1 + 2); };  \
      \
			infixExpression(); \
			",
      "(1 + 2)"
    },
    { "\
			let reverse = macro(a, b) { quote(unquote(b) - unquote(a)); }; \
      \
			reverse(2 + 2, 10 - 5); \
			",
      "(10 - 5) - (2 + 2)"
    },
    { "\
			let unless = macro(condition, consequence, alternative) { \
				quote(if (!(unquote(condition))) { \
					unquote(consequence); \
				} else { \
					unquote(alternative); \
				}); \
			}; \
      \
			unless(10 > 5, puts(\"not greater\"), puts(\"greater\")); \
      \
			",
      "if (!(10 > 5)) { puts(\"not greater\") } else { puts(\"greater\") }"
    }
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      auto expected = test_parse_program(c.expected);
      auto program = test_parse_program(c.input);
      auto env = make_shared<Environment>();
      define_macros(program, env);
      auto expanded = expand_macros(program, env);
      REQUIRE(expanded->to_string() == expected->to_string());
    });
}
