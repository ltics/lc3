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

auto test_integer_literal(shared_ptr<Expression> expr, TestVariant value) -> bool {
  shared_ptr<IntegerLiteral> int_expr = static_pointer_cast<IntegerLiteral>(expr);
  return int_expr->value == value.as_int;
}

auto test_identifier(shared_ptr<Expression> expr, TestVariant value) -> bool {
  shared_ptr<Identifier> str_expr = static_pointer_cast<Identifier>(expr);
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
    return test_identifier(expr, expected);
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
  struct TestCase {
    string input;
    string expected_identifier;
    TestVariant expected_value;
  };

  TestVariant v_int = TestVariant(5);
  TestVariant v_bool = TestVariant(true);
  string s("y");
  TestVariant v_str = TestVariant(s);

  vector<TestCase> tests = {
    TestCase{ "let x = 5;", "x", v_int },
    TestCase{ "let y = true;", "y", v_bool },
    TestCase{ "let foobar = y;", "foobar", v_str }
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
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

TEST_CASE("test parse return statements") {
  struct TestCase {
    string input;
    TestVariant expected_value;
  };

  TestVariant v_int = TestVariant(5);
  TestVariant v_bool = TestVariant(true);
  string s("foobar");
  TestVariant v_str = TestVariant(s);

  vector<TestCase> tests = {
    TestCase{ "return 5;", v_int },
    TestCase{ "return true;", v_bool },
    TestCase{ "return foobar;", v_str }
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      auto lexer = Lexer::new_lexer(c.input);
      auto parser = Parser::new_parser(lexer);

      auto program = parser->parse_program();
      check_parse_errors(parser);

      REQUIRE(program->statements.size() == 1);

      shared_ptr<ReturnStatement> stmt = static_pointer_cast<ReturnStatement>(program->statements[0]);
      REQUIRE(stmt->token_literal() == "return");

      REQUIRE(test_literal_expression(stmt->value, c.expected_value));
    });
}

TEST_CASE("test parse identifier") {
  struct TestCase {
    string input;
    TestVariant expected_value;
  };

  TestVariant v_int = TestVariant(5);
  TestVariant v_bool_true = TestVariant(true);
  TestVariant v_bool_false = TestVariant(false);
  string s("foobar");
  TestVariant v_str = TestVariant(s);

  vector<TestCase> tests = {
    TestCase{ "5;", v_int },
    TestCase{ "true;", v_bool_true },
    TestCase{ "false;", v_bool_false },
    TestCase{ "foobar;", v_str }
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      auto lexer = Lexer::new_lexer(c.input);
      auto parser = Parser::new_parser(lexer);

      auto program = parser->parse_program();
      check_parse_errors(parser);

      REQUIRE(program->statements.size() == 1);

      shared_ptr<ExpressionStatement> stmt = static_pointer_cast<ExpressionStatement>(program->statements[0]);
      REQUIRE(test_literal_expression(stmt->expression, c.expected_value));
    });
}

TEST_CASE("test parse prefix expression") {
  struct TestCase {
    string input;
    string expected_operator;
    TestVariant expected_value;
  };

  TestVariant v_int1 = TestVariant(5);
  TestVariant v_int2 = TestVariant(15);
  TestVariant v_bool_true = TestVariant(true);
  TestVariant v_bool_false = TestVariant(false);
  string s("foobar");
  TestVariant v_str = TestVariant(s);

  vector<TestCase> tests = {
    TestCase{ "!5;", "!", v_int1 },
    TestCase{ "-15", "-", v_int2 },
    TestCase{ "!true;", "!", v_bool_true },
    TestCase{ "!false;", "!", v_bool_false },
    TestCase{ "!foobar;", "!", v_str },
    TestCase{ "-foobar;", "-", v_str }
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      auto lexer = Lexer::new_lexer(c.input);
      auto parser = Parser::new_parser(lexer);

      auto program = parser->parse_program();
      check_parse_errors(parser);

      REQUIRE(program->statements.size() == 1);
      shared_ptr<ExpressionStatement> stmt = static_pointer_cast<ExpressionStatement>(program->statements[0]);
      shared_ptr<PrefixExpression> expr = static_pointer_cast<PrefixExpression>(stmt->expression);
      REQUIRE(expr->prefix_operator == c.expected_operator);
      REQUIRE(test_literal_expression(expr->right, c.expected_value));
    });
}

TEST_CASE("test parse infix expression") {
  struct TestCase {
    string input;
    TestVariant expected_left_value;
    string expected_operator;
    TestVariant expected_right_value;
  };

  TestVariant v_int = TestVariant(5);
  TestVariant v_bool_true = TestVariant(true);
  TestVariant v_bool_false = TestVariant(false);
  string s1("foobar");
  TestVariant v_str1 = TestVariant(s1);
  string s2("barfoo");
  TestVariant v_str2 = TestVariant(s2);

  vector<TestCase> tests = {
    TestCase{ "5 + 5;", v_int, "+", v_int },
		TestCase{ "5 - 5;", v_int, "-", v_int },
		TestCase{ "5 * 5;", v_int, "*", v_int },
		TestCase{ "5 / 5;", v_int, "/", v_int },
		TestCase{ "5 > 5;", v_int, ">", v_int },
		TestCase{ "5 < 5;", v_int, "<", v_int },
		TestCase{ "5 == 5;", v_int, "==", v_int },
		TestCase{ "5 != 5;", v_int, "!=", v_int },
		TestCase{ "foobar + barfoo;", v_str1, "+", v_str2 },
		TestCase{ "foobar - barfoo;", v_str1, "-", v_str2 },
		TestCase{ "foobar * barfoo;", v_str1, "*", v_str2 },
		TestCase{ "foobar / barfoo;", v_str1, "/", v_str2 },
		TestCase{ "foobar > barfoo;", v_str1, ">", v_str2 },
		TestCase{ "foobar < barfoo;", v_str1, "<", v_str2 },
		TestCase{ "foobar == barfoo;", v_str1, "==", v_str2 },
		TestCase{ "foobar != barfoo;", v_str1, "!=", v_str2 },
		TestCase{ "true == true", v_bool_true, "==", v_bool_true },
		TestCase{ "true != false", v_bool_true, "!=", v_bool_false },
		TestCase{ "false == false", v_bool_false, "==", v_bool_false }
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      auto lexer = Lexer::new_lexer(c.input);
      auto parser = Parser::new_parser(lexer);

      auto program = parser->parse_program();
      check_parse_errors(parser);

      REQUIRE(program->statements.size() == 1);
      shared_ptr<ExpressionStatement> stmt = static_pointer_cast<ExpressionStatement>(program->statements[0]);
      shared_ptr<InfixExpression> expr = static_pointer_cast<InfixExpression>(stmt->expression);
      REQUIRE(expr->infix_operator == c.expected_operator);
      REQUIRE(test_literal_expression(expr->left, c.expected_left_value));
      REQUIRE(test_literal_expression(expr->right, c.expected_right_value));
    });
}
