#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "../src/object.hpp"
#include "../src/quote_unquote.hpp"
#include "./util.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <memory>

using namespace std;
using namespace ast;
using namespace object;
using namespace quoteunquote;
using namespace testutil;

TEST_CASE("test quote") {
  struct TestCase {
    string input;
    string expected;
  };

  vector<TestCase> tests = {
    { "quote(5)", "5" },
    { "quote(5 + 8)", "(5 + 8)" },
    { "quote(foobar)", "foobar" },
    { "quote(foobar + barfoo)", "(foobar + barfoo)" }
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      auto evaluated = test_eval(c.input);
      REQUIRE(evaluated->type() == QUOTE_OBJ);
      auto quote = static_pointer_cast<Quote>(evaluated);
      REQUIRE(quote->node != nullptr);
      REQUIRE(quote->node->to_string() == c.expected);
    });
}

TEST_CASE("test quote unquote") {
  struct TestCase {
    string input;
    string expected;
  };

  vector<TestCase> tests = {
    { "quote(unquote(4))", "4" },
    { "quote(unquote(4 + 4))", "8" },
    { "quote(8 + unquote(4 + 4))", "(8 + 8)" },
    { "quote(unquote(4 + 4) + 8)", "(8 + 8)" },
    { "let foobar = 8; quote(foobar)", "foobar" },
    { "let foobar = 8; quote(unquote(foobar))", "8" },
    { "quote(unquote(true))", "true" },
    { "quote(unquote(true == false))", "false" },
    { "quote(unquote(quote(4 + 4)))", "(4 + 4)" },
    { "let quotedInfixExpression = quote(4 + 4); \
			 quote(unquote(4 + 4) + unquote(quotedInfixExpression))",
      "(8 + (4 + 4))"
    }
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      auto evaluated = test_eval(c.input);
      REQUIRE(evaluated->type() == QUOTE_OBJ);
      auto quote = static_pointer_cast<Quote>(evaluated);
      REQUIRE(quote->node != nullptr);
      REQUIRE(quote->node->to_string() == c.expected);
    });
}
