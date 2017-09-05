#include "ast.hpp"
#include "token.hpp"
#include "lexer.hpp"
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <functional>

using namespace std;
using namespace std::placeholders;

namespace parser {
  typedef std::function<shared_ptr<ast::Expression>()> prefix_parse_fn;
  typedef std::function<shared_ptr<ast::Expression>(shared_ptr<ast::Expression>)> infix_parse_fn;

  enum class Precedence : size_t {
      LOWEST
    , EQUALS      // ==
    , LESSGREATER // > or <
    , SUMMINUS    // + or -
    , MULDIV      // * or /
    , PREFIX      // -x or !x
    , APPLY       // func(x)
    , INDEX       // arr[i]
  };

  map<token::TokenType, Precedence> precedences = {
    { token::EQ, Precedence::EQUALS },
    { token::NOT_EQ, Precedence::EQUALS },
    { token::LT, Precedence::LESSGREATER },
    { token::GT, Precedence::LESSGREATER },
    { token::PLUS, Precedence::SUMMINUS },
    { token::MINUS, Precedence::SUMMINUS },
    { token::SLASH, Precedence::MULDIV },
    { token::ASTERISK, Precedence::MULDIV },
    { token::LPAREN, Precedence::APPLY },
    { token::LBRACKET, Precedence::INDEX }
  };

  class Parser {
  private:
    shared_ptr<lexer::Lexer> lexer;
    token::Token current_token;
    token::Token peek_token;
    vector<string> errors;
    map<token::TokenType, prefix_parse_fn> prefix_parse_fns;
    map<token::TokenType, infix_parse_fn> infix_parse_fns;

  public:
    Parser(shared_ptr<lexer::Lexer> l);

    auto get_prefix_parse_fns() -> map<token::TokenType, prefix_parse_fn>;
    auto get_infix_parse_fns() -> map<token::TokenType, infix_parse_fn>;
    auto next_token() -> void;
    auto current_token_is(token:: TokenType tt) -> bool;
    auto peek_token_is(token::TokenType tt) -> bool;
    auto expect_peek(token::TokenType tt) -> bool;
    auto get_errors() -> vector<string>;
    auto peek_error(token::TokenType tt) -> void;
    auto no_prefix_parse_fn_error(token::TokenType tt) -> void;

    auto peek_precedence() -> Precedence;
    auto current_precedence() -> Precedence;

    auto parse_program() -> shared_ptr<ast::Program>;
    auto parse_statement() -> shared_ptr<ast::Statement>;
    auto parse_let_statement() -> shared_ptr<ast::LetStatement>;
    auto parse_return_statement() -> shared_ptr<ast::ReturnStatement>;
    auto parse_expression_statement() -> shared_ptr<ast::ExpressionStatement>;
    auto parse_expression(Precedence prec) -> shared_ptr<ast::Expression>;
    auto parse_identifier() -> shared_ptr<ast::Expression>;
    auto parse_integer_literal() -> shared_ptr<ast::Expression>;
    auto parse_string_literal() -> shared_ptr<ast::Expression>;
    auto parse_prefix_expression() -> shared_ptr<ast::Expression>;
    auto parse_infix_expression(shared_ptr<ast::Expression> left) -> shared_ptr<ast::Expression>;
    auto parse_boolean() -> shared_ptr<ast::Expression>;
    auto parse_grouped_expression() -> shared_ptr<ast::Expression>;
    auto parse_if_expression() -> shared_ptr<ast::Expression>;
    auto parse_block_statement() -> shared_ptr<ast::BlockStatement>;
    auto parse_function_literal() -> shared_ptr<ast::Expression>;
    auto parse_function_parameters() -> vector<shared_ptr<ast::Identifier>>;
    auto parse_call_expression(shared_ptr<ast::Expression> func) -> shared_ptr<ast::Expression>;
    auto parse_expression_list(token::TokenType end) -> vector<shared_ptr<ast::Expression>>;
    auto parse_array_literal() -> shared_ptr<ast::Expression>;
    auto parse_index_expression(shared_ptr<ast::Expression> left) -> shared_ptr<ast::Expression>;
    auto parse_hash_literal() -> shared_ptr<ast::Expression>;

    auto register_prefix(token::TokenType tt, prefix_parse_fn f) -> void;
    auto register_infix(token::TokenType tt, infix_parse_fn f) -> void;

    static auto new_parser(shared_ptr<lexer::Lexer> l) -> shared_ptr<Parser>;
  };

  Parser::Parser(shared_ptr<lexer::Lexer> l) {
    this->lexer = l;
    this->errors = {};
    this->prefix_parse_fns = {};
    this->register_prefix(token::IDENT, std::bind(&Parser::parse_identifier, this));
    this->register_prefix(token::INT, std::bind(&Parser::parse_integer_literal, this));
    this->register_prefix(token::STRING, std::bind(&Parser::parse_string_literal, this));
    this->register_prefix(token::BANG, std::bind(&Parser::parse_prefix_expression, this));
    this->register_prefix(token::MINUS,std::bind(&Parser::parse_prefix_expression, this));
    this->register_prefix(token::TRUET, std::bind(&Parser::parse_boolean, this));
    this->register_prefix(token::FALSET, std::bind(&Parser::parse_boolean, this));
    this->register_prefix(token::LPAREN, std::bind(&Parser::parse_grouped_expression, this));
    this->register_prefix(token::IF, std::bind(&Parser::parse_if_expression, this));
    this->register_prefix(token::FUNCTION, std::bind(&Parser::parse_function_literal, this));
    this->register_prefix(token::LBRACKET, std::bind(&Parser::parse_array_literal, this));
    this->register_prefix(token::LBRACE, std::bind(&Parser::parse_hash_literal, this));

    this->infix_parse_fns = {};
    this->register_infix(token::PLUS, std::bind(&Parser::parse_infix_expression, this, _1));
    this->register_infix(token::MINUS, std::bind(&Parser::parse_infix_expression, this, _1));
    this->register_infix(token::SLASH, std::bind(&Parser::parse_infix_expression, this, _1));
    this->register_infix(token::ASTERISK, std::bind(&Parser::parse_infix_expression, this, _1));
    this->register_infix(token::EQ, std::bind(&Parser::parse_infix_expression, this, _1));
    this->register_infix(token::NOT_EQ, std::bind(&Parser::parse_infix_expression, this, _1));
    this->register_infix(token::LT, std::bind(&Parser::parse_infix_expression, this, _1));
    this->register_infix(token::GT, std::bind(&Parser::parse_infix_expression, this, _1));
    this->register_infix(token::LPAREN, std::bind(&Parser::parse_call_expression, this, _1));
    this->register_infix(token::LBRACE, std::bind(&Parser::parse_index_expression, this, _1));
  }

  auto Parser::new_parser(shared_ptr<lexer::Lexer> l) -> shared_ptr<Parser> {
    shared_ptr<Parser> p = make_shared<Parser>(l);
    p->next_token();
    p->next_token();
    return p;
  }

  auto Parser::get_prefix_parse_fns() -> map<token::TokenType, prefix_parse_fn> {
    return this->prefix_parse_fns;
  }

  auto Parser::get_infix_parse_fns() -> map<token::TokenType, infix_parse_fn> {
    return this->infix_parse_fns;
  }

  auto Parser::next_token() -> void {
    this->current_token = this->peek_token;
    this->peek_token = this->lexer->next_token();
  }

  auto Parser::current_token_is(token::TokenType tt) -> bool {
    return this->current_token.type == tt;
  }

  auto Parser::peek_token_is(token::TokenType tt) -> bool {
    return this->peek_token.type == tt;
  }

  auto Parser::expect_peek(token::TokenType tt) -> bool {
    if (this->peek_token_is(tt)) {
      this->next_token();
      return true;
    } else {
      this->peek_error(tt);
      return false;
    }
  }

  auto Parser::get_errors() -> vector<string> {
    return this->errors;
  }

  auto Parser::peek_error(token::TokenType tt) -> void {
    string msg("");
    msg += "expected next token to be ";
    msg += tt;
    msg += ", got ";
    msg += this->peek_token.type;
    msg += " instead";
    this->errors.push_back(msg);
  }

  auto Parser::no_prefix_parse_fn_error(token::TokenType tt) -> void {
    string msg("");
    msg += "no prefix parse function for ";
    msg += tt;
    msg += " found";
    this->errors.push_back(msg);
  }

  auto Parser::register_prefix(token::TokenType tt, prefix_parse_fn f) -> void {
    this->prefix_parse_fns[tt] = f;
  }

  auto Parser::register_infix(token::TokenType tt, infix_parse_fn f) -> void {
    this->infix_parse_fns[tt] = f;
  }

  auto Parser::peek_precedence() -> Precedence {
    return precedences[this->peek_token.type];
  }

  auto Parser::current_precedence() -> Precedence {
    return precedences[this->current_token.type];
  }

  auto Parser::parse_program() -> shared_ptr<ast::Program> {
    vector<shared_ptr<ast::Statement>> statements = {};
    while(!this->current_token_is(token::EOFT)) {
      auto stmt = this->parse_statement();
      if (stmt != nullptr) {
        statements.push_back(stmt);
      }
      this->next_token();
    }
    return make_shared<ast::Program>(statements);
  }

  auto Parser::parse_statement() -> shared_ptr<ast::Statement> {
    auto tt = this->current_token.type;
    if (tt == token::LET) {
      return this->parse_let_statement();
    } else if (tt == token::RETURN) {
      return this->parse_return_statement();
    } else {
      return this->parse_expression_statement();
    }
  }

  auto Parser::parse_let_statement() -> shared_ptr<ast::LetStatement> {
    auto current_token = this->current_token;

    if (!this->expect_peek(token::IDENT)) {
      return nullptr;
    }

    auto name = make_shared<ast::Identifier>(this->current_token, this->current_token.literal);

    if (!this->expect_peek(token::ASSIGN)) {
      return nullptr;
    }

    this->next_token();
    auto value = this->parse_expression(Precedence::LOWEST);

    if (this->peek_token_is(token::SEMICOLON)) {
      this->next_token();
    }

    return make_shared<ast::LetStatement>(current_token, name, value);
  }

  auto Parser::parse_return_statement() -> shared_ptr<ast::ReturnStatement> {
    auto current_token = this->current_token;
    this->next_token();
    auto value = this->parse_expression(Precedence::LOWEST);

    if (this->peek_token_is(token::SEMICOLON)) {
      this->next_token();
    }

    return make_shared<ast::ReturnStatement>(current_token, value);
  }

  auto Parser::parse_expression_statement() -> shared_ptr<ast::ExpressionStatement> {
    auto current_token = this->current_token;
    auto expr = this->parse_expression(Precedence::LOWEST);

    if (this->peek_token_is(token::SEMICOLON)) {
      this->next_token();
    }

    return make_shared<ast::ExpressionStatement>(current_token, expr);
  }

  auto Parser::parse_expression(Precedence prec) -> shared_ptr<ast::Expression> {
    auto prefix_fn = this->prefix_parse_fns[this->current_token.type];
    if (prefix_fn == nullptr) {
      this->no_prefix_parse_fn_error(this->current_token.type);
      return nullptr;
    }

    auto left_expr = prefix_fn();

    while (!this->peek_token_is(token::SEMICOLON) && prec < this->peek_precedence()) {
      auto infix_fn = this->infix_parse_fns[this->peek_token.type];
      if (infix_fn == nullptr) {
        return nullptr;
      }

      this->next_token();

      left_expr = infix_fn(left_expr);
    }
    return left_expr;
  }

  auto Parser::parse_identifier() -> shared_ptr<ast::Expression> {
    return make_shared<ast::Identifier>(this->current_token, this->current_token.literal);
  }

  auto Parser::parse_integer_literal() -> shared_ptr<ast::Expression> {
    auto current_token = this->current_token;

    int value;
    try {
      value = stoi(this->current_token.literal);
    } catch (const std::exception& e) {
      string msg("");
      msg += "could not parse ";
      msg += this->current_token.literal;
      msg += " as integer";
      this->errors.push_back(msg);
      return nullptr;
    }

    return make_shared<ast::IntegerLiteral>(current_token, value);
  }

  auto Parser::parse_string_literal() -> shared_ptr<ast::Expression> {
    return make_shared<ast::StringLiteral>(this->current_token, this->current_token.literal);
  }

  auto Parser::parse_prefix_expression() -> shared_ptr<ast::Expression> {
    auto current_token = this->current_token;

    this->next_token();
    auto right = this->parse_expression(Precedence::PREFIX);

    return make_shared<ast::PrefixExpression>(current_token, current_token.literal, right);
  }

  auto Parser::parse_infix_expression(shared_ptr<ast::Expression> left) -> shared_ptr<ast::Expression> {
    auto current_token = this->current_token;

    auto prec = this->current_precedence();
    this->next_token();
    auto right = this->parse_expression(prec);

    return make_shared<ast::InfixExpression>(current_token, left, current_token.literal, right);
  }

  auto Parser::parse_boolean() -> shared_ptr<ast::Expression> {
    return make_shared<ast::Boolean>(this->current_token, this->current_token_is(token::TRUET));
  }

  auto Parser::parse_grouped_expression() -> shared_ptr<ast::Expression> {
    this->next_token();

    auto expr = this->parse_expression(Precedence::LOWEST);

    if (this->expect_peek(token::RPAREN)) {
      return nullptr;
    }

    return expr;
  }

  auto Parser::parse_if_expression() -> shared_ptr<ast::Expression> {
    auto current_token = this->current_token;

    if (!this->expect_peek(token::LPAREN)) {
      return nullptr;
    }

    this->next_token();

    auto condition = this->parse_expression(Precedence::LOWEST);

    if (!this->expect_peek(token::RPAREN)) {
      return nullptr;
    }

    if (!this->expect_peek(token::LBRACE)) {
      return nullptr;
    }

    auto consequence = this->parse_block_statement();
    shared_ptr<ast::BlockStatement> alternative = nullptr;

    if (this->peek_token_is(token::ELSE)) {
      this->next_token();

      if (!this->expect_peek(token::LBRACE)) {
        return nullptr;
      }

      alternative = this->parse_block_statement();
    }

    return make_shared<ast::IfExpression>(current_token, condition, consequence, alternative);
  }

  auto Parser::parse_block_statement() -> shared_ptr<ast::BlockStatement> {
    auto current_token = this->current_token;

    vector<shared_ptr<ast::Statement>> statements = {};
    this->next_token();

    while (!this->current_token_is(token::RBRACE) && !this->current_token_is(token::EOFT)) {
      auto stmt = this->parse_statement();
      if (stmt != nullptr) {
        statements.push_back(stmt);
      }
      this->next_token();
    }

    return make_shared<ast::BlockStatement>(current_token, statements);
  }

  auto Parser::parse_function_literal() -> shared_ptr<ast::Expression> {
    auto current_token = this->current_token;

    if (!this->expect_peek(token::LPAREN)) {
      return nullptr;
    }

    auto parameters = this->parse_function_parameters();

    if (!this->expect_peek(token::LBRACE)) {
      return nullptr;
    }

    auto body = this->parse_block_statement();

    return make_shared<ast::FunctionLiteral>(current_token, parameters, body);
  }

  auto Parser::parse_function_parameters() -> vector<shared_ptr<ast::Identifier>> {
    return {};
  }

  auto Parser::parse_call_expression(shared_ptr<ast::Expression> func) -> shared_ptr<ast::Expression> {
    auto current_token = this->current_token;
    auto arguments = this->parse_expression_list(token::RPAREN);
    return make_shared<ast::CallExpression>(current_token, func, arguments);
  }

  auto Parser::parse_expression_list(token::TokenType end) -> vector<shared_ptr<ast::Expression>> {
    return {};
  }

  auto Parser::parse_array_literal() -> shared_ptr<ast::Expression> {
    return nullptr;
  }

  auto Parser::parse_index_expression(shared_ptr<ast::Expression> left) -> shared_ptr<ast::Expression> {
    return nullptr;
  }

  auto Parser::parse_hash_literal() -> shared_ptr<ast::Expression> {
    return nullptr;
  }
}
