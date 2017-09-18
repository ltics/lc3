#pragma once

#include "token.hpp"
#include <memory>
#include <vector>
#include <algorithm>
#include <range/v3/all.hpp>

using namespace std;
using namespace ranges;

namespace ast {
  enum class NodeType : size_t {
    STATEMENT,
    EXPRESSION,
    PROGRAM,
    IDENTIFIER,
    BOOLEAN,
    INTEGERLITERAL,
    STRINGLITERAL,
    PREFIXEXPRESSION,
    INFIXEXPRESSION,
    LETSTATEMENT,
    RETURNSTATEMENT,
    EXPRESSIONSTATEMENT,
    BLOCKSTATEMENT,
    IFEXPRESSION,
    FUNCTIONLITERAL,
    CALLEXPRESSION,
    ARRAYLITERAL,
    INDEXEXPRESSION,
    HASHLITERAL
  };

  class Node {
  protected:
    token::Token token;

  public:
    Node() {
      token = {"empty", "empty"};
    };
    Node(token::Token t) {
      token = t;
    };
    virtual NodeType type() = 0;
    virtual string token_literal() = 0;
    virtual string to_string() = 0;
  };

  class Statement : public Node {
  public:
    Statement(): Node() {};
    Statement(token::Token t): Node(t) {};

    NodeType type() {
      return NodeType::STATEMENT;
    }

    string token_literal() {
      return "statement literal";
    }

    string to_string() {
      return "statement node";
    }
  };

  class Expression : public Node {
  public:
    Expression(): Node() {};
    Expression(token::Token t): Node(t) {};

    NodeType type() {
      return NodeType::EXPRESSION;
    }

    string token_literal() {
      return "expression literal";
    }

    string to_string() {
      return "expression node";
    }
  };

  auto flatten_expressions(vector<shared_ptr<Expression>> strs) -> string {
    if (strs.size() == 0) {
      return "";
    } else if (strs.size() == 1) {
      return strs[0]->to_string();
    } else {
      vector<string> literals = strs | view::take(strs.size() - 1) | view::transform([](shared_ptr<Expression> a) {
          auto s = a->to_string();
          s += ",";
          return s;
        });
      literals.push_back(strs[strs.size() - 1]->to_string());
      return literals | view::join(' ');
    }
  }

  class Program : public Node {
  public:
    vector<shared_ptr<Statement>> statements = {};
    Program(vector<shared_ptr<Statement>> stms): statements(stms) {};

    NodeType type() {
      return NodeType::PROGRAM;
    }

    string token_literal() {
      if (this->statements.size() > 0) {
        return this->statements[0]->token_literal();
      } else {
        return "empty program";
      }
    }

    string to_string() {
      string s("");
      std::for_each(statements.cbegin(), statements.cend(), [&](shared_ptr<Statement> stm) {
          s += stm->to_string();
        });
      return s;
    }
  };

  class Identifier : public Expression {
  public:
    string value;

    Identifier(token::Token t, string v): Expression(t), value(v) {};

    NodeType type() {
      return NodeType::IDENTIFIER;
    }

    string token_literal() {
      return this->token.literal;
    }

    string to_string() {
      return this->value;
    }
  };

  class Boolean : public Expression {
  public:
    bool value;

    Boolean(token::Token t, bool v): Expression(t), value(v) {};

    NodeType type() {
      return NodeType::BOOLEAN;
    }

    auto get_value() -> bool {
      return this->value;
    }

    string token_literal() {
      return this->token.literal;
    }

    string to_string() {
      return this->token_literal();
    }
  };

  class IntegerLiteral : public Expression {
  public:
    int value;

    IntegerLiteral(token::Token t, int v): Expression(t), value(v) {};

    NodeType type() {
      return NodeType::INTEGERLITERAL;
    }

    string token_literal() {
      return this->token.literal;
    }

    string to_string() {
      return this->token_literal();
    }
  };

  class StringLiteral : public Expression {
  public:
    string value;

    StringLiteral(token::Token t, string v): Expression(t), value(v) {};

    NodeType type() {
      return NodeType::STRINGLITERAL;
    }

    string token_literal() {
      return this->token.literal;
    }

    string to_string() {
      return this->token_literal();
    }
  };

  class PrefixExpression : public Expression {
  public:
    string prefix_operator;
    shared_ptr<Expression> right;

    PrefixExpression(token::Token t,
                     string o,
                     shared_ptr<Expression> r)
      : Expression(t), prefix_operator(o), right(r) {};

    NodeType type() {
      return NodeType::PREFIXEXPRESSION;
    }

    string token_literal() {
      return this->token.literal;
    }

    string to_string() {
      string s("");
      s += "(";
      s += this->prefix_operator;
      s += this->right->to_string();
      s += ")";
      return s;
    }
  };

  class InfixExpression : public Expression {
  public:
    shared_ptr<Expression> left;
    string infix_operator;
    shared_ptr<Expression> right;

    InfixExpression(token::Token t,
                    shared_ptr<Expression> l,
                    string o, shared_ptr<Expression> r)
      : Expression(t), left(l), infix_operator(o), right(r) {};

    NodeType type() {
      return NodeType::INFIXEXPRESSION;
    }

    string token_literal() {
      return this->token.literal;
    }

    string to_string() {
      string s("");
      s += "(";
      s += this->left->to_string();
      s += " ";
      s += this->infix_operator;
      s += " ";
      s += this->right->to_string();
      s += ")";
      return s;
    }
  };

  class LetStatement : public Statement {
  public:
    shared_ptr<Identifier> name;
    shared_ptr<Expression> value;

    LetStatement(token::Token t, shared_ptr<Identifier> n, shared_ptr<Expression> v): Statement(t), name(n), value(v) {};

    NodeType type() {
      return NodeType::LETSTATEMENT;
    }

    string token_literal() {
      return this->token.literal;
    }
    string to_string() {
      string s("");
      s += this->token_literal();
      s += " ";
      s += this->name->to_string();
      s += " = ";
      if (this->value != nullptr) {
        s += this->value->to_string();
      }
      s += ";";
      return s;
    }
  };

  class ReturnStatement : public Statement {
  public:
    shared_ptr<Expression> value;

    ReturnStatement(token::Token t, shared_ptr<Expression> v): Statement(t), value(v) {};

    NodeType type() {
      return NodeType::RETURNSTATEMENT;
    }

    string token_literal() {
      return this->token.literal;
    }

    string to_string() {
      string s("");
      s += this->token_literal();
      s += " ";
      if (this->value != nullptr) {
        s += this->value->to_string();
      }
      s += ";";
      return s;
    }
  };

  // Just a statement wrapper for toplevel expressions
  class ExpressionStatement : public Statement {
  public:
    shared_ptr<Expression> expression;

    ExpressionStatement(token::Token t, shared_ptr<Expression> expr): Statement(t), expression(expr) {};

    NodeType type() {
      return NodeType::EXPRESSIONSTATEMENT;
    }

    string token_literal() {
      return this->token.literal;
    }

    string to_string() {
      if (this->expression != nullptr) {
        return this->expression->to_string();
      } else {
        return "";
      }
    }
  };

  class BlockStatement : public Statement {
  public:
    vector<shared_ptr<Statement>> statements;

    BlockStatement(token::Token t, vector<shared_ptr<Statement>> stms): Statement(t), statements(stms) {};

    NodeType type() {
      return NodeType::BLOCKSTATEMENT;
    }

    string token_literal() {
      return this->token.literal;
    }

    string to_string() {
      string s("");
      std::for_each(statements.cbegin(), statements.cend(), [&](shared_ptr<Statement> stm) {
          s += stm->to_string();
        });
      return s;
    }
  };

  class IfExpression : public Expression {
  public:
    shared_ptr<Expression> condition;
    shared_ptr<BlockStatement> consequence;
    shared_ptr<BlockStatement> alternative;

    IfExpression(token::Token t, shared_ptr<Expression> cond, shared_ptr<BlockStatement> cons, shared_ptr<BlockStatement> alt): Expression(t), condition(cond), consequence(cons), alternative(alt) {};

    NodeType type() {
      return NodeType::IFEXPRESSION;
    }

    string token_literal() {
      return this->token.literal;
    }

    string to_string() {
      string s("");
      s += "if";
      s += this->condition->to_string();
      s += " ";
      s += this->consequence->to_string();
      if (this->alternative != nullptr) {
        s += "else";
        s += " ";
        s += this->alternative->to_string();
      }
      return s;
    }
  };

  class FunctionLiteral : public Expression {
  public:
    vector<shared_ptr<Identifier>> parameters;
    shared_ptr<BlockStatement> body;

    FunctionLiteral(token::Token t, vector<shared_ptr<Identifier>> ps, shared_ptr<BlockStatement> b): Expression(t), parameters(ps), body(b) {};

    NodeType type() {
      return NodeType::FUNCTIONLITERAL;
    }

    string token_literal() {
      return this->token.literal;
    }

    string to_string() {
      string s("");

      string literal = flatten_expressions(parameters | view::transform([](shared_ptr<Identifier> a) { return static_pointer_cast<Expression>(a); }));

      s += this->token_literal();
      s += "(";
      s += literal;
      s += ")";
      s += " ";
      s += this->body->to_string();
      return s;
    }
  };

  class CallExpression : public Expression {
  public:
    shared_ptr<Expression> function;
    vector<shared_ptr<Expression>> arguments;

    CallExpression(token::Token t, shared_ptr<Expression> f, vector<shared_ptr<Expression>> args): Expression(t), function(f), arguments(args) {};

    NodeType type() {
      return NodeType::CALLEXPRESSION;
    }

    string token_literal() {
      return this->token.literal;
    }

    string to_string() {
      string s("");

      string literal = flatten_expressions(arguments);

      s += this->function->to_string();
      s += "(";
      s += literal;
      s += ")";
      return s;
    }
  };

  class ArrayLiteral : public Expression {
  public:
    vector<shared_ptr<Expression>> elements;

    ArrayLiteral(token::Token t, vector<shared_ptr<Expression>> elems): Expression(t), elements(elems) {};

    NodeType type() {
      return NodeType::ARRAYLITERAL;
    }

    string token_literal() {
      return this->token.literal;
    }

    string to_string() {
      string s("");

      string literal = flatten_expressions(elements);

      s += "[";
      s += literal;
      s += "]";
      return s;
    }
  };

  class IndexExpression : public Expression {
  public:
    shared_ptr<Expression> left;
    shared_ptr<Expression> index;

    IndexExpression(token::Token t, shared_ptr<Expression> l, shared_ptr<Expression> i): Expression(t), left(l), index(i) {};

    NodeType type() {
      return NodeType::INDEXEXPRESSION;
    }

    string token_literal() {
      return this->token.literal;
    }

    string to_string() {
      string s("");

      s += "(";
      s += this->left->to_string();
      s += "[";
      s += this->index->to_string();
      s += "])";
      return s;
    }
  };

  class HashLiteral : public Expression {
  public:
    map<shared_ptr<Expression>, shared_ptr<Expression>> pairs;

    HashLiteral(token::Token t, map<shared_ptr<Expression>, shared_ptr<Expression>> ps): Expression(t), pairs(ps) {};

    NodeType type() {
      return NodeType::HASHLITERAL;
    }

    string token_literal() {
      return this->token.literal;
    }

    string to_string() {
      string s("");

      vector<string> literals = pairs | view::transform([](pair<shared_ptr<Expression>, shared_ptr<Expression>> const &p) { return p.first->to_string() + ":" + p.second->to_string(); });
      string literal = literals | view::join(',');

      s += "{";
      s += literal;
      s += "}";
      return s;
    }
  };
}
