#pragma once

#include <map>
#include <string>

using namespace std;

namespace token {
  typedef struct {
    string type;
    string literal;
  } Token;


  const string ILLEGAL = "ILLEGAL";
  const string EOFT = "EOF";

  // Identifiers + literals
  const string IDENT = "IDENT"; // add, foobar, x, y, ...
  const string INT = "INT"; // 1343456
  const string STRING = "STRING"; // "foobar"

  // Operators
  const string ASSIGN = "=";
  const string PLUS = "+";
  const string MINUS = "-";
  const string BANG = "!";
  const string ASTERISK = "*";
  const string SLASH = "/";
  const string LT = "<";
  const string GT = ">";
  const string EQ = "==";

  // Delimiters
  const string COMMA = ",";
  const string SEMICOLON = ";";
  const string COLON = ":";
  const string LPAREN = "(";
  const string RPAREN = ")";
  const string LBRACE = "{";
  const string RBRACE = "}";
  const string LBRACKET = "{";
  const string RBRACKET = "}";

  // Keywords
  const string FUNCTION = "FUNCTION";
  const string LET = "LET";
  const string TRUET = "TRUE";
  const string FALSET = "FALSE";
  const string IF = "IF";
  const string ELSE = "ELSE";
  const string RETURN = "RETURN";

  map<string, string> token_type = {
    { "fn", FUNCTION },
    { "let", LET },
    { "true", TRUET },
    { "false", FALSET },
    { "if", IF },
    { "else", ELSE },
    { "return", RETURN }
  };

  auto lookup_indent_type(string ident) -> string {
    auto ident_type = token_type[ident];
    if (ident_type == "") {
      return IDENT;
    } else {
      return ident_type;
    }
  }
}

