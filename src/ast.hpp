#include "token.hpp"
#include <memory>
#include <vector>

using namespace std;

class Node {
public:
  virtual string token_literal() = 0;
  virtual string to_string() = 0;
};

class Statement : public Node {
public:
  string token_literal() {
    return "statement literal";
  }

  string to_string() {
    return "statement node";
  }
};

class Expression : public Node {
public:
  string token_literal() {
    return "expression literal";
  }

  string to_string() {
    return "expression node";
  }
};

typedef struct {
  vector<Statement> statements;
} Program;

class Identifier : public Expression {
private:
  token::Token token;
  string value;

public:
  string token_literal() {
    return this->token.literal;
  }

  string to_string() {
    return this->value;
  }
};

class LetStatement : public Statement {
private:
  token::Token token;
  shared_ptr<Identifier> name;
  shared_ptr<Expression> value;

public:
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

