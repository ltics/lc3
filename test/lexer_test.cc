#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <numeric>
#include "../src/token.hpp"
#include "../src/lexer.hpp"

using namespace std;

TEST_CASE("binary_search") {
  vector<string> inputs = {
    "let five = 5;\n",
    "let ten = 10;\n",
    "\n",
    "let add = fn(x, y) {\n",
    "  x + y;\n",
    "};\n",
    "\n",
    "let result = add(five, ten);\n",
    "!-/*5;\n",
    "5 < 10 > 5;\n",
    "\n",
    "if (5 < 10) {\n",
    "\treturn true;\n",
    "} else {\n",
    "\treturn false;\n",
    "}\n",
    "\n",
    "10 == 10;\n",
    "10 != 9;\n",
    "\"foobar\"\n",
    "\"foo bar\"\n",
    "[1, 2];\n",
    "{\"foo\": \"bar\"}"
  };

  string input;
  input = accumulate(inputs.cbegin(), inputs.cend(), input);

  shared_ptr<lexer::Lexer> l = lexer::Lexer::new_lexer(input);

  vector<token::Token> tests = {
    { token::LET, "let" },
    { token::IDENT, "five" },
    { token::ASSIGN, "=" },
    { token::INT, "5" },
    { token::SEMICOLON, ";" },
    { token::LET, "let" },
    { token::IDENT, "ten" },
    { token::ASSIGN, "=" },
    { token::INT, "10" },
    { token::SEMICOLON, ";" },
    { token::LET, "let" },
    { token::IDENT, "add" },
    { token::ASSIGN, "=" },
    { token::FUNCTION, "fn" },
    { token::LPAREN, "(" },
    { token::IDENT, "x" },
    { token::COMMA, "," },
    { token::IDENT, "y" },
    { token::RPAREN, ")" },
    { token::LBRACE, "{" },
    { token::IDENT, "x" },
    { token::PLUS, "+" },
    { token::IDENT, "y" },
    { token::SEMICOLON, ";" },
    { token::RBRACE, "}" },
    { token::SEMICOLON, ";" },
    { token::LET, "let" },
    { token::IDENT, "result" },
    { token::ASSIGN, "=" },
    { token::IDENT, "add" },
    { token::LPAREN, "(" },
    { token::IDENT, "five" },
    { token::COMMA, "," },
    { token::IDENT, "ten" },
    { token::RPAREN, ")" },
    { token::SEMICOLON, ";" },
    { token::BANG, "!" },
    { token::MINUS, "-" },
    { token::SLASH, "/" },
    { token::ASTERISK, "*" },
    { token::INT, "5" },
    { token::SEMICOLON, ";" },
    { token::INT, "5" },
    { token::LT, "<" },
    { token::INT, "10" },
    { token::GT, ">" },
    { token::INT, "5" },
    { token::SEMICOLON, ";" },
    { token::IF, "if" },
    { token::LPAREN, "(" },
    { token::INT, "5" },
    { token::LT, "<" },
    { token::INT, "10" },
    { token::RPAREN, ")" },
    { token::LBRACE, "{" },
    { token::RETURN, "return" },
    { token::TRUET, "true" },
    { token::SEMICOLON, ";" },
    { token::RBRACE, "}" },
    { token::ELSE, "else" },
    { token::LBRACE, "{" },
    { token::RETURN, "return" },
    { token::FALSET, "false" },
    { token::SEMICOLON, ";" },
    { token::RBRACE, "}" },
    { token::INT, "10" },
    { token::EQ, "==" },
    { token::INT, "10" },
    { token::SEMICOLON, ";" },
    { token::INT, "10" },
    { token::NOT_EQ, "!=" },
    { token::INT, "9" },
    { token::SEMICOLON, ";" },
    { token::STRING, "foobar" },
    { token::STRING, "foo bar" },
    { token::LBRACKET, "[" },
    { token::INT, "1" },
    { token::COMMA, "," },
    { token::INT, "2" },
    { token::RBRACKET, "]" },
    { token::SEMICOLON, ";" },
    { token::LBRACE, "{" },
    { token::STRING, "foo" },
    { token::COLON, ":" },
    { token::STRING, "bar" },
    { token::RBRACE, "}" },
    { token::EOFT, string(1, '\0') } // literally string(1, '\0') and "" are identical, but dunno why can not pass the equality checking
  };

  for_each(tests.cbegin(), tests.cend(), [=](token::Token t) -> void {
      token::Token tok = l -> next_token();
      REQUIRE(tok.type == t.type);
      REQUIRE(tok.literal == t.literal);
    });
}
