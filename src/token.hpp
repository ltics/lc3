#pragma once

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

  // Operators
  const string ASSIGN = "=";
  const string PLUS = "+";

  // Delimiters
  const string COMMA = ",";
  const string SEMICOLON = ";";
  const string LPAREN = "(";
  const string RPAREN = ")";
  const string LBRACE = "{";
  const string RBRACE = "}";

  // Keywords
  const string FUNCTION = "FUNCTION";
  const string LET = "LET";
}

