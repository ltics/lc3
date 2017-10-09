#include "catch.hpp"
#include "../src/token.hpp"
#include "../src/ast.hpp"
#include <vector>
#include <string>
#include <memory>

using namespace std;
using namespace ast;

TEST_CASE("ast test") {
  token::Token myVarToken = { token::IDENT, "myVar" };
  shared_ptr<Identifier> lvar = make_shared<Identifier>(myVarToken, "lvar");
  token::Token anotherVarToken = { token::IDENT, "anotherVar"};
  shared_ptr<Identifier> rvar = make_shared<Identifier>(anotherVarToken, "rvar");
  token::Token letToken = { token::LET, "let" };
  shared_ptr<LetStatement> let = make_shared<LetStatement>(letToken, lvar, rvar);
  vector<shared_ptr<Statement>> statements = { let };
  shared_ptr<Program> program = make_shared<Program>(statements);
  REQUIRE(lvar->to_string() == "lvar");
  REQUIRE(rvar->to_string() == "rvar");
  REQUIRE(let->to_string() == "let lvar = rvar;");
  REQUIRE(program->to_string() == "let lvar = rvar;");
}
