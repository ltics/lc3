#include "ast.hpp"
#include "token.hpp"
#include "lexer.hpp"
#include <memory>

namespace parser {
  class Parser {
  private:
    shared_ptr<lexer::Lexer> lexer;
    token::Token current_token;
    token::Token peek_token;

  public:
    Parser(shared_ptr<lexer::Lexer> l);

    auto next_token() -> void;
    auto parse_program() -> shared_ptr<ast::Program>;

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
    this->peek_token = lexer::next_token(this->lexer);
  }

  auto Parser::parse_program() -> shared_ptr<ast::Program> {
    return nullptr;
  }
}
