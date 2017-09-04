#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "../src/token.hpp"
#include "../src/lexer.hpp"
#include "../src/parser.hpp"
#include <vector>
#include <string>

using namespace std;
using namespace token;
using namespace lexer;
using namespace parser;

TEST_CASE("parser test") {
  REQUIRE(Precedence::EQUALS > Precedence::LOWEST);
  REQUIRE(precedences[EQ] == Precedence::EQUALS);
  REQUIRE(precedences["cleantha"] == Precedence::LOWEST);

  auto l = Lexer::new_lexer("");
  auto p = Parser::new_parser(l);
  REQUIRE(p->get_prefix_parse_fns()["cleantha"] == nullptr);
  REQUIRE(p->get_infix_parse_fns()[LBRACE] != nullptr);
}
