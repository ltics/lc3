#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "../src/token.hpp"

using namespace std;

TEST_CASE("binary_search") {
  REQUIRE(token::lookup_indent_type("fn") == token::FUNCTION);
  REQUIRE(token::lookup_indent_type("cleantha") == token::IDENT);
}
