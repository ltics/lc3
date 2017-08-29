#include "token.hpp"
#include <memory>
#include <vector>
#include <algorithm>

using namespace std;

namespace ast {
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
    virtual string token_literal() = 0;
    virtual string to_string() = 0;
  };

  class Statement : public Node {
  public:
    Statement(): Node() {};
    Statement(token::Token t): Node(t) {};
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
    string token_literal() {
      return "expression literal";
    }

    string to_string() {
      return "expression node";
    }
  };

  class Program : public Node {
  private:
    vector<Statement> statements = {};

  public:
    Program(vector<Statement> stms): statements(stms) {};

    string token_literal() {
      if (this->statements.size() > 0) {
        return this->statements[0].token_literal();
      } else {
        return "empty program";
      }
    }

    string to_string() {
      string s("");
      for_each(statements.cbegin(), statements.cend(), [&](Statement stm) {
          s += stm.to_string();
        });
      return s;
    }
  };

  class Identifier : public Expression {
  private:
    string value;

  public:
    Identifier(token::Token t, string v): Expression(t), value(v) {};

    string token_literal() {
      return this->token.literal;
    }

    string to_string() {
      return this->value;
    }
  };

  class LetStatement : public Statement {
  private:
    shared_ptr<Identifier> name;
    shared_ptr<Expression> value;

  public:
    LetStatement(token::Token t, shared_ptr<Identifier> n, shared_ptr<Expression> v): Statement(t), name(n), value(v) {};

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
  private:
    shared_ptr<Expression> value;

  public:
    ReturnStatement(token::Token t, shared_ptr<Expression> v): Statement(t), value(v) {};

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
  private:
    shared_ptr<Expression> expression;

  public:
    ExpressionStatement(token::Token t, shared_ptr<Expression> expr): Statement(t), expression(expr) {};

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
  private:
    vector<Statement> statements;

  public:
    BlockStatement(token::Token t, vector<Statement> stms): Statement(t), statements(stms) {};

    string token_literal() {
      return this->token.literal;
    }

    string to_string() {
      string s("");
      for_each(statements.cbegin(), statements.cend(), [&](Statement stm) {
          s += stm.to_string();
        });
      return s;
    }
  };
}
