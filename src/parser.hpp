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
}
