#pragma once

#include "token.hpp"
#include <string>
#include <memory>

using namespace std;

namespace lexer {
  typedef struct {
    string input;
    unsigned long position; // current position in input (points to current char)
    unsigned long read_position; // current reading position in input (after current char)
    char ch; // current char under examination
  } Lexer;

  auto read_char(shared_ptr<Lexer> l) -> void {
    if (l->read_position >= l->input.size()) {
      l->ch = '\0';
    } else {
      l->ch = l->input.at(l->read_position);
    }
    l->position = l->read_position;
    l->read_position += 1;
  }

  auto new_token(string tokenType, char ch) -> token::Token {
    string ch_s(1, ch);
    return { tokenType, ch_s };
  }

  auto next_token(shared_ptr<Lexer> l) -> token::Token {
    token::Token tok;

    switch (l->ch) {
    case '=':
      tok = new_token(token::ASSIGN, l->ch);
      break;
    case ';':
      tok = new_token(token::SEMICOLON, l->ch);
      break;
    case '(':
      tok = new_token(token::LPAREN, l->ch);
      break;
    case ')':
      tok = new_token(token::RPAREN, l->ch);
      break;
    case ',':
      tok = new_token(token::COMMA, l->ch);
      break;
    case '+':
      tok = new_token(token::PLUS, l->ch);
      break;
    case '{':
      tok = new_token(token::LBRACE, l->ch);
      break;
    case '}':
      tok = new_token(token::RBRACE, l->ch);
      break;
    default:
      tok = new_token(token::EOFT, l->ch);
    }

    read_char(l);
    return tok;
  }

  auto new_lexer(string input) -> shared_ptr<Lexer> {
    shared_ptr<Lexer> l = shared_ptr<Lexer>(new Lexer({ input, 0, 0, 0 }));
    read_char(l);
    return l;
  };
}

