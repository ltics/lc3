#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "../src/token.hpp"
#include "../src/ast.hpp"
#include "../src/lexer.hpp"
#include "../src/parser.hpp"
#include "./util.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace token;
using namespace ast;
using namespace lexer;
using namespace parser;
using namespace testutil;

auto check_parse_errors(shared_ptr<Parser> p) -> void {
  auto errors = p->get_errors();
  if (errors.size() == 0) {
    return;
  }

  cout << "parser has " << errors.size() << " errors" << endl;
  std::for_each(errors.cbegin(), errors.cend(), [](string error) -> void {
      cout << "parser error: " << error << endl;
    });
  return;
}

struct TestCase {
  string input;
  string expected_identifier;
  TestVariant expected_value;
};

auto test_integer_literal(shared_ptr<Expression> expr, TestVariant value) -> bool {
  shared_ptr<IntegerLiteral> int_expr = static_pointer_cast<IntegerLiteral>(expr);
  return int_expr->value == value.as_int;
}

auto test_string_literal(shared_ptr<Expression> expr, TestVariant value) -> bool {
  shared_ptr<StringLiteral> str_expr = static_pointer_cast<StringLiteral>(expr);
  return str_expr->value == value.as_string;
}

auto test_boolean_literal(shared_ptr<Expression> expr, TestVariant value) -> bool {
  shared_ptr<Boolean> bool_expr = static_pointer_cast<Boolean>(expr);
  return bool_expr->value == value.as_bool;
}

auto test_literal_expression(shared_ptr<Expression> expr, TestVariant expected) -> bool {
  switch (expected.type_id) {
  case TestVariant::t_int:
    return test_integer_literal(expr, expected);
    break;
  case TestVariant::t_bool:
    return test_boolean_literal(expr, expected);
    break;
  case TestVariant::t_string:
    return test_string_literal(expr, expected);
    break;
  default:
    return false;
  }
}

TEST_CASE("test precedence") {
  REQUIRE(Precedence::EQUALS > Precedence::LOWEST);
  REQUIRE(precedences[EQ] == Precedence::EQUALS);
  REQUIRE(precedences["cleantha"] == Precedence::LOWEST);

  auto l = Lexer::new_lexer("");
  auto p = Parser::new_parser(l);
  REQUIRE(p->get_prefix_parse_fns()["cleantha"] == nullptr);
  REQUIRE(p->get_infix_parse_fns()[LBRACE] != nullptr);
}

TEST_CASE("test parse let statements") {
  TestVariant v_int = TestVariant(5);
  TestVariant v_bool = TestVariant(true);
  string s("y");
  TestVariant v_str = TestVariant(s);

  vector<TestCase> tests = {
    TestCase{ "let x = 5;", "x", v_int },
    TestCase{ "let y = true;", "y", v_bool },
    TestCase{ "let foobar = y;", "foobar", v_str }
  };

  std::for_each(tests.cbegin(), tests.cend(), [&](TestCase c) {
      auto lexer = Lexer::new_lexer(c.input);
      auto parser = Parser::new_parser(lexer);

      auto program = parser->parse_program();
      check_parse_errors(parser);

      REQUIRE(program->statements.size() == 1);

      shared_ptr<LetStatement> stmt = static_pointer_cast<LetStatement>(program->statements[0]);
      REQUIRE(stmt->token_literal() == "let");
      REQUIRE(stmt->name->value == c.expected_identifier);

      REQUIRE(test_literal_expression(stmt->value, c.expected_value) == true);
    });
}
