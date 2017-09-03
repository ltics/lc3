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

TEST_CASE("binary_search") {
  REQUIRE(precedences[EQ] == Precedence::EQUALS);
  REQUIRE(precedences["cleantha"] == Precedence::LOWEST);
}
