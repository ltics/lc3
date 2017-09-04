#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "../src/token.hpp"
#include <range/v3/all.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace ranges;

TEST_CASE("token test") {
  vector<string> ss = {"1", "2", "3"};
  string s = ss | view::join(',');

  REQUIRE(s == "1,2,3");
  REQUIRE(token::lookup_indent_type("fn") == token::FUNCTION);
  REQUIRE(token::lookup_indent_type("cleantha") == token::IDENT);
}
