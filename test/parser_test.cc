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
  REQUIRE(p->get_infix_parse_fns()[LBRACKET] != nullptr);
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

TEST_CASE("test parse operator precedence") {
  struct TestCase {
    string input;
    string expected;
  };

  vector<TestCase> tests = {
    {
        "-a * b",
        "((-a) * b)",
    },
    {
        "!-a",
        "(!(-a))",
    },
    {
        "a + b + c",
        "((a + b) + c)",
    },
    {
        "a + b - c",
        "((a + b) - c)",
    },
    {
        "a * b * c",
        "((a * b) * c)",
    },
    {
        "a * b / c",
        "((a * b) / c)",
    },
    {
        "a + b / c",
        "(a + (b / c))",
    },
    {
        "a + b * c + d / e - f",
        "(((a + (b * c)) + (d / e)) - f)",
    },
    {
        "3 + 4; -5 * 5",
        "(3 + 4)((-5) * 5)",
    },
    {
        "5 > 4 == 3 < 4",
        "((5 > 4) == (3 < 4))",
    },
    {
        "5 < 4 != 3 > 4",
        "((5 < 4) != (3 > 4))",
    },
    {
        "3 + 4 * 5 == 3 * 1 + 4 * 5",
        "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
    },
    {
        "true",
        "true",
    },
    {
        "false",
        "false",
    },
    {
        "3 > 5 == false",
        "((3 > 5) == false)",
    },
    {
        "3 < 5 == true",
        "((3 < 5) == true)",
    },
    {
        "1 + (2 + 3) + 4",
        "((1 + (2 + 3)) + 4)",
    },
    {
        "(5 + 5) * 2",
        "((5 + 5) * 2)",
    },
    {
        "2 / (5 + 5)",
        "(2 / (5 + 5))",
    },
    {
        "(5 + 5) * 2 * (5 + 5)",
        "(((5 + 5) * 2) * (5 + 5))",
    },
    {
        "-(5 + 5)",
        "(-(5 + 5))",
    },
    {
        "!(true == true)",
        "(!(true == true))",
    },
    {
        "a + add(b * c) + d",
        "((a + add((b * c))) + d)",
    },
    {
        "add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
        "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))",
    },
    {
        "add(a + b + c * d / f + g)",
        "add((((a + b) + ((c * d) / f)) + g))",
    },
    {
        "a * [1, 2, 3, 4][b * c] * d",
        "((a * ([1, 2, 3, 4][(b * c)])) * d)",
    },
    {
        "add(a * b[2], b[1], 2 * [1, 2][1])",
        "add((a * (b[2])), (b[1]), (2 * ([1, 2][1])))",
    }
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      auto lexer = Lexer::new_lexer(c.input);
      auto parser = Parser::new_parser(lexer);

      shared_ptr<Program> program = parser->parse_program();
      check_parse_errors(parser);
      REQUIRE(program->to_string() == c.expected);
    });
}

TEST_CASE("test parse if expression") {
  auto input = "if (x < y) { x }";
  auto lexer = Lexer::new_lexer(input);
  auto parser = Parser::new_parser(lexer);

  shared_ptr<Program> program = parser->parse_program();
  check_parse_errors(parser);
  REQUIRE(program->statements.size() == 1);
  shared_ptr<ExpressionStatement> stmt = static_pointer_cast<ExpressionStatement>(program->statements[0]);
  shared_ptr<IfExpression> expr = static_pointer_cast<IfExpression>(stmt->expression);

  shared_ptr<InfixExpression> cond = static_pointer_cast<InfixExpression>(expr->condition);
  REQUIRE(cond->infix_operator == "<");
  string s_left("x");
  TestVariant v_left = TestVariant(s_left);
  string s_right("y");
  TestVariant v_right = TestVariant(s_right);

  REQUIRE(test_literal_expression(cond->left, v_left));
  REQUIRE(test_literal_expression(cond->right, v_right));

  REQUIRE(expr->consequence->statements.size() == 1);
  shared_ptr<ExpressionStatement> cons = static_pointer_cast<ExpressionStatement>(expr->consequence->statements[0]);
  string c("x");
  TestVariant v_c = TestVariant(c);
  REQUIRE(test_identifier(cons->expression, v_c));

  REQUIRE(expr->alternative == nullptr);
}

TEST_CASE("test parse if else expression") {
  auto input = "if (x < y) { x } else { y }";
  auto lexer = Lexer::new_lexer(input);
  auto parser = Parser::new_parser(lexer);

  shared_ptr<Program> program = parser->parse_program();
  check_parse_errors(parser);
  REQUIRE(program->statements.size() == 1);
  shared_ptr<ExpressionStatement> stmt = static_pointer_cast<ExpressionStatement>(program->statements[0]);
  shared_ptr<IfExpression> expr = static_pointer_cast<IfExpression>(stmt->expression);

  shared_ptr<InfixExpression> cond = static_pointer_cast<InfixExpression>(expr->condition);
  REQUIRE(cond->infix_operator == "<");
  string s_left("x");
  TestVariant v_left = TestVariant(s_left);
  string s_right("y");
  TestVariant v_right = TestVariant(s_right);

  REQUIRE(test_literal_expression(cond->left, v_left));
  REQUIRE(test_literal_expression(cond->right, v_right));

  REQUIRE(expr->consequence->statements.size() == 1);
  shared_ptr<ExpressionStatement> cons = static_pointer_cast<ExpressionStatement>(expr->consequence->statements[0]);
  string c("x");
  TestVariant v_c = TestVariant(c);
  REQUIRE(test_identifier(cons->expression, v_c));

  REQUIRE(expr->alternative->statements.size() == 1);
  shared_ptr<ExpressionStatement> alt = static_pointer_cast<ExpressionStatement>(expr->alternative->statements[0]);
  string a("y");
  TestVariant v_a = TestVariant(a);
  REQUIRE(test_identifier(alt->expression, v_a));
}

TEST_CASE("test parse function literal") {
  auto input = "fn(x, y) { x + y; }";
  auto lexer = Lexer::new_lexer(input);
  auto parser = Parser::new_parser(lexer);

  shared_ptr<Program> program = parser->parse_program();
  check_parse_errors(parser);
  REQUIRE(program->statements.size() == 1);

  shared_ptr<ExpressionStatement> stmt = static_pointer_cast<ExpressionStatement>(program->statements[0]);
  shared_ptr<FunctionLiteral> func = static_pointer_cast<FunctionLiteral>(stmt->expression);

  REQUIRE(func->parameters.size() == 2);

  string p1("x");
  TestVariant v_p1 = TestVariant(p1);
  string p2("y");
  TestVariant v_p2 = TestVariant(p2);
  REQUIRE(test_literal_expression(func->parameters[0], v_p1));
  REQUIRE(test_literal_expression(func->parameters[1], v_p2));

  REQUIRE(func->body->statements.size() == 1);
  shared_ptr<ExpressionStatement> body_stmt = static_pointer_cast<ExpressionStatement>(func->body->statements[0]);

  shared_ptr<InfixExpression> expr = static_pointer_cast<InfixExpression>(body_stmt->expression);
  REQUIRE(expr->infix_operator == "+");
  string s_left("x");
  TestVariant v_left = TestVariant(s_left);
  string s_right("y");
  TestVariant v_right = TestVariant(s_right);

  REQUIRE(test_literal_expression(expr->left, v_left));
  REQUIRE(test_literal_expression(expr->right, v_right));
}

TEST_CASE("test parse function parameters") {
  struct TestCase {
    string input;
    vector<string> expected_params;
  };

  vector<TestCase> tests = {
    TestCase{ "fn() {};", {} },
    TestCase{ "fn(x) {};", { "x" } },
    TestCase{ "fn(x, y, z) {};", { "x", "y", "z" } },
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      auto lexer = Lexer::new_lexer(c.input);
      auto parser = Parser::new_parser(lexer);

      auto program = parser->parse_program();
      check_parse_errors(parser);
      REQUIRE(program->statements.size() == 1);

      shared_ptr<ExpressionStatement> stmt = static_pointer_cast<ExpressionStatement>(program->statements[0]);
      shared_ptr<FunctionLiteral> func = static_pointer_cast<FunctionLiteral>(stmt->expression);
      REQUIRE(func->parameters.size() == c.expected_params.size());

      for (int i = 0; i < c.expected_params.size(); i++) {
        test_literal_expression(func->parameters.at(i), TestVariant(c.expected_params[i]));
      }
    });
}
