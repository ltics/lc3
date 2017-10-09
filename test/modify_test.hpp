#include "catch.hpp"
#include "../src/token.hpp"
#include "../src/ast.hpp"
#include "../src/modify.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <memory>

using namespace token;
using namespace std;
using namespace ast;
using namespace modify;

auto turn_one_into_two(shared_ptr<Node> node) -> shared_ptr<Node> {
  if (node->type() != NodeType::INTEGERLITERAL) {
    return node;
  }

  auto int_node = static_pointer_cast<IntegerLiteral>(node);

  if (int_node->value != 1) {
    return node;
  }

  int_node->value = 2;
  return int_node;
}

TEST_CASE("test modify") {
  auto one = [=]() -> shared_ptr<Expression> {
    return make_shared<IntegerLiteral>(Token({ INT, "1" }), 1);
  };
  auto two = [=]() -> shared_ptr<Expression> {
    return make_shared<IntegerLiteral>(Token({ INT, "2" }), 2);
  };

  struct TestCase {
    shared_ptr<Node> input;
    shared_ptr<Node> expected;
  };

  Token t1 = { INT, "1" };
  Token t2 = { INT, "2" };
  Token tr = { RETURN, "return" };
  Token tl = { LET, "let" };
  Token tf = { FUNCTION, "fn" };
  Token ta = { LBRACE, "{" };
  Token th = { LBRACKET, "[" };

  vector<TestCase> tests = {
    { one(), two() },
    { make_shared<Program>(vector<shared_ptr<Statement>>({ make_shared<ExpressionStatement>(t1, one()) })),
      make_shared<Program>(vector<shared_ptr<Statement>>({ make_shared<ExpressionStatement>(t2, two()) }))
    },
    { make_shared<InfixExpression>(t1, one(), "+", two()),
      make_shared<InfixExpression>(t2, two(), "+", two())
    },
    { make_shared<InfixExpression>(t1, two(), "+", one()),
      make_shared<InfixExpression>(t2, two(), "+", two())
    },
    { make_shared<PrefixExpression>(t1, "-", one()),
      make_shared<PrefixExpression>(t2, "-", two())
    },
    { make_shared<IndexExpression>(t1, one(), one()),
      make_shared<IndexExpression>(t2, two(), two())
    },
    { make_shared<IfExpression>(t1,
                                one(),
                                make_shared<BlockStatement>(t1, vector<shared_ptr<Statement>>({ make_shared<ExpressionStatement>(t1, one()) })),
                                make_shared<BlockStatement>(t1, vector<shared_ptr<Statement>>({ make_shared<ExpressionStatement>(t1, one()) }))),
      make_shared<IfExpression>(t1,
                                two(),
                                make_shared<BlockStatement>(t2, vector<shared_ptr<Statement>>({ make_shared<ExpressionStatement>(t2, two()) })),
                                make_shared<BlockStatement>(t2, vector<shared_ptr<Statement>>({ make_shared<ExpressionStatement>(t2, two()) })))
    },
    { make_shared<ReturnStatement>(tr, one()),
      make_shared<ReturnStatement>(tr, two())
    },
    { make_shared<LetStatement>(tl, make_shared<Identifier>(t2, "two"), one()),
      make_shared<LetStatement>(tl, make_shared<Identifier>(t2, "two"), two())
    },
    { make_shared<FunctionLiteral>(tf, vector<shared_ptr<Identifier>>({}),
                                   make_shared<BlockStatement>(t1, vector<shared_ptr<Statement>>({ make_shared<ExpressionStatement>(t1, one()) }))),
      make_shared<FunctionLiteral>(tf, vector<shared_ptr<Identifier>>({}),
                                   make_shared<BlockStatement>(t2, vector<shared_ptr<Statement>>({ make_shared<ExpressionStatement>(t2, two()) }))) 
    },
    { make_shared<ArrayLiteral>(ta, vector<shared_ptr<Expression>>({ one(), one() })),
      make_shared<ArrayLiteral>(ta, vector<shared_ptr<Expression>>({ two(), two() }))
    },
    { make_shared<HashLiteral>(th, map<shared_ptr<Expression>, shared_ptr<Expression>>({ { one(), one() }, { one(), one() } })),
      make_shared<HashLiteral>(th, map<shared_ptr<Expression>, shared_ptr<Expression>>({ { two(), two() }, { two(), two() } })),
    }
  };

  std::for_each(tests.cbegin(), tests.cend(), [](TestCase c) {
      auto modified = modify::modify(c.input, turn_one_into_two);
      REQUIRE(modified->to_string() == c.expected->to_string());
    });
}
