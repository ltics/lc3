#pragma once

#include "token.hpp"
#include <string>
#include <memory>

using namespace std;

namespace lexer {
  class Lexer {
  private:
    string input;
    unsigned long position; // current position in input (points to current char)
    unsigned long read_position; // current reading position in input (after current char)
    char ch; // current char under examination

  public:
    Lexer(string input);

    auto read_char() -> void;
    auto peek_char() -> char;
    auto skip_whitespace() -> void;
    auto read_identifier() -> string;
    auto read_number() -> string;
    auto read_string() -> string;
    auto next_token() -> token::Token;

    static auto is_letter(char ch) -> bool;
    static auto is_digit(char ch) -> bool;
    static auto new_token(string tokenType, char ch) -> token::Token;
    static auto new_lexer(string input) -> shared_ptr<Lexer>;
  };

  Lexer::Lexer(string input) {
    this->input = input;
    this->position = 0;
    this->read_position = 0;
    this->ch = '\0';
  }

  auto Lexer::read_char() -> void {
    if (this->read_position >= this->input.size()) {
      this->ch = '\0';
    } else {
      this->ch = this->input.at(this->read_position);
    }
    this->position = this->read_position;
    this->read_position += 1;
  }

  auto Lexer::peek_char() -> char {
    if (this->read_position >= this->input.size()) {
      return '\0';
    } else {
      return this->input.at(this->read_position);
    }
  }

  auto Lexer::skip_whitespace() -> void {
    while (this->ch == ' ' || this->ch == '\t' || this->ch == '\n' || this->ch == '\r') {
      this->read_char();
    }
  }

  auto Lexer::is_letter(char ch) -> bool {
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
  }

  auto Lexer::is_digit(char ch) -> bool {
    return '0' <= ch && ch <= '9';
  }

  auto Lexer::read_identifier() -> string {
    auto position = this->position;
    while (is_letter(this->ch)) {
      this->read_char();
    }
    return this->input.substr(position, this->position - position);
  }

  auto Lexer::read_number() -> string {
    auto position = this->position;
    while (is_digit(this->ch)) {
      this->read_char();
    }
    return this->input.substr(position, this->position - position);
  }

  auto Lexer::read_string() -> string {
    auto position = this->position + 1;
    while (true) {
      this->read_char();
      if (this->ch == '"' || this->ch == '\0') {
        break;
      }
    }
    return this->input.substr(position, this->position - position);
  }

  auto Lexer::new_token(string tokenType, char ch) -> token::Token {
    string ch_s(1, ch);
    return { tokenType, ch_s };
  }

  auto Lexer::next_token() -> token::Token {
    token::Token tok;

    this->skip_whitespace();

    switch (this->ch) {
    case '=':
      if (this->peek_char() == '=') {
        this->read_char();
        tok = { token::EQ, "==" };
      } else {
        tok = new_token(token::ASSIGN, this->ch);
      }
      break;
    case '+':
      tok = new_token(token::PLUS, this->ch);
      break;
    case '-':
      tok = new_token(token::MINUS, this->ch);
      break;
    case '!':
      if (this->peek_char() == '=') {
        this->read_char();
        tok = { token::NOT_EQ, "!=" };
      } else {
        tok = new_token(token::BANG, this->ch);
      }
      break;
    case '/':
      tok = new_token(token::SLASH, this->ch);
      break;
    case '*':
      tok = new_token(token::ASTERISK, this->ch);
      break;
    case '<':
      tok = new_token(token::LT, this->ch);
      break;
    case '>':
      tok = new_token(token::GT, this->ch);
      break;
    case ';':
      tok = new_token(token::SEMICOLON, this->ch);
      break;
    case ':':
      tok = new_token(token::COLON, this->ch);
      break;
    case ',':
      tok = new_token(token::COMMA, this->ch);
      break;
    case '(':
      tok = new_token(token::LPAREN, this->ch);
      break;
    case ')':
      tok = new_token(token::RPAREN, this->ch);
      break;
    case '{':
      tok = new_token(token::LBRACE, this->ch);
      break;
    case '}':
      tok = new_token(token::RBRACE, this->ch);
      break;
    case '[':
      tok = new_token(token::LBRACKET, this->ch);
      break;
    case ']':
      tok = new_token(token::RBRACKET, this->ch);
      break;
    case '"':
      tok = { token::STRING, this->read_string() };
      break;
    case '\0':
      tok = new_token(token::EOFT, this->ch);
      break;
    default:
      if (is_letter(this->ch)) {
        auto literal = this->read_identifier();
        return { token::lookup_indent_type(literal), literal };
      } else if (is_digit(this->ch)) {
        return { token::INT, this->read_number() };
      } else {
        tok = new_token(token::ILLEGAL, this->ch);
      }
    }

    this->read_char();
    return tok;
  }

  auto Lexer::new_lexer(string input) -> shared_ptr<Lexer> {
    shared_ptr<Lexer> l = shared_ptr<Lexer>(new Lexer(input));
    l->read_char();
    return l;
  };
}

