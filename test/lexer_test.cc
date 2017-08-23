#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include "../src/token.hpp"
#include "../src/lexer.hpp"

using namespace std;

TEST_CASE("binary_search") {
  string input = "=+(){},;";
  shared_ptr<lexer::Lexer> l = lexer::new_lexer(input);

  vector<token::Token> tests = {
    { token::ASSIGN, "=" },
    { token::PLUS, "+" },
    { token::LPAREN, "(" },
    { token::RPAREN, ")" },
    { token::LBRACE, "{" },
    { token::RBRACE, "}" },
    { token::COMMA, "," },
    { token::SEMICOLON, ";" },
    { token::EOFT, string(1, '\0') } // literally string(1, '\0') and "" are identical, but dunno why can not pass the equality checking
  };

  for_each(tests.cbegin(), tests.cend(), [=](token::Token t) -> void {
      token::Token tok = lexer::next_token(l);
      REQUIRE(tok.type == t.type);
      REQUIRE(tok.literal == t.literal);
    });
}
