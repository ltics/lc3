#pragma once

#include <map>
#include <string>

using namespace std;

namespace token {
  typedef string TokenType;
  typedef string TokenLiteral;

  typedef struct {
    TokenType type;
    TokenLiteral literal;
  } Token;


  const TokenType ILLEGAL = "ILLEGAL";
  const TokenType EOFT = "EOF";

  // Identifiers + literals
  const TokenType IDENT = "IDENT"; // add, foobar, x, y, ...
  const TokenType INT = "INT"; // 1343456
  const TokenType STRING = "STRING"; // "foobar"

  // Operators
  const TokenType ASSIGN = "=";
  const TokenType PLUS = "+";
  const TokenType MINUS = "-";
  const TokenType BANG = "!";
  const TokenType ASTERISK = "*";
  const TokenType SLASH = "/";
  const TokenType LT = "<";
  const TokenType GT = ">";
  const TokenType EQ = "==";
  const TokenType NOT_EQ = "!=";

  // Delimiters
  const TokenType COMMA = ",";
  const TokenType SEMICOLON = ";";
  const TokenType COLON = ":";
  const TokenType LPAREN = "(";
  const TokenType RPAREN = ")";
  const TokenType LBRACE = "{";
  const TokenType RBRACE = "}";
  const TokenType LBRACKET = "[";
  const TokenType RBRACKET = "]";

  // Keywords
  const TokenType FUNCTION = "FUNCTION";
  const TokenType LET = "LET";
  const TokenType TRUET = "TRUE";
  const TokenType FALSET = "FALSE";
  const TokenType IF = "IF";
  const TokenType ELSE = "ELSE";
  const TokenType RETURN = "RETURN";

  map<TokenLiteral, TokenType> token_type = {
    { "fn", FUNCTION },
    { "let", LET },
    { "true", TRUET },
    { "false", FALSET },
    { "if", IF },
    { "else", ELSE },
    { "return", RETURN }
  };

  auto lookup_indent_type(TokenLiteral ident) -> TokenType {
    auto ident_type = token_type[ident];
    if (ident_type == "") {
      return IDENT;
    } else {
      return ident_type;
    }
  }
}

