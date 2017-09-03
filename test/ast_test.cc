#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "../src/token.hpp"
#include "../src/ast.hpp"
#include <vector>
#include <string>

using namespace std;
using namespace ast;

TEST_CASE("binary_search") {
  shared_ptr<Identifier> lvar = shared_ptr<Identifier>(new Identifier({ token::IDENT, "myVar" }, "lvar"));
  shared_ptr<Identifier> rvar = shared_ptr<Identifier>(new Identifier({ token::IDENT, "anotherVar"}, "rvar"));
  shared_ptr<LetStatement> let = shared_ptr<LetStatement>(new LetStatement({ token::LET, "let" }, lvar, rvar));
  shared_ptr<Program> program = shared_ptr<Program>(new Program({ let }));
  REQUIRE(lvar->to_string() == "lvar");
  REQUIRE(rvar->to_string() == "rvar");
  REQUIRE(let->to_string() == "let lvar = rvar;");
  REQUIRE(program->to_string() == "let lvar = rvar;");
}
