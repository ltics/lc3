#include "ast.hpp"
#include "token.hpp"
#include "lexer.hpp"
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <functional>

using namespace std;

namespace parser {
  typedef std::function<ast::Expression()> prefix_parse_fn;
  typedef std::function<ast::Expression(ast::Expression)> infix_parse_fn;

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

    auto next_token() -> void;
    auto parse_program() -> shared_ptr<ast::Program>;

    auto register_prefix(token::TokenType tt, prefix_parse_fn f) -> void;
    auto register_infix(token::TokenType tt, infix_parse_fn f) -> void;

    static auto new_parser(shared_ptr<lexer::Lexer> l) -> shared_ptr<Parser>;
  };

  Parser::Parser(shared_ptr<lexer::Lexer> l) {
    this->lexer = l;
    this->errors = {};
    this->prefix_parse_fns = {};
    this->infix_parse_fns = {};
  }

  auto Parser::new_parser(shared_ptr<lexer::Lexer> l) -> shared_ptr<Parser> {
    shared_ptr<Parser> p = shared_ptr<Parser>(new Parser(l));
    p->next_token();
    p->next_token();
    return p;
  }

  auto Parser::next_token() -> void {
    this->current_token = this->peek_token;
    this->peek_token = this->lexer->next_token();
  }

  auto Parser::parse_program() -> shared_ptr<ast::Program> {
    return nullptr;
  }

  auto Parser::register_prefix(token::TokenType tt, prefix_parse_fn f) -> void {
    this->prefix_parse_fns[tt] = f;
  }

  auto Parser::register_infix(token::TokenType tt, infix_parse_fn f) -> void {
    this->infix_parse_fns[tt] = f;
  }
}
