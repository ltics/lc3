#pragma once

#include <iostream>
#include <stdlib.h>
#include <editline/readline.h>
#include <fstream>
#include <iterator>
#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "object.hpp"
#include "eval.hpp"
#include "macro_expansion.hpp"

using namespace std;
using namespace ast;
using namespace lexer;
using namespace parser;
using namespace object;
using namespace eval;
using namespace macroexpansion;

namespace interpret {
  auto check_parser_errors(shared_ptr<Parser> p) -> bool {
    auto errors = p->get_errors();
    if (errors.size() == 0) {
      return false;
    }

    cout << "parser has " << errors.size() << " errors" << endl;
    std::for_each(errors.cbegin(), errors.cend(), [](string error) -> void {
        cout << "parser error: " << error << endl;
      });
    return true;
  }

  auto interp(const string &input, shared_ptr<Environment> env, shared_ptr<Environment> macro_env) -> void {
    shared_ptr<Lexer> l = Lexer::new_lexer(input);
    shared_ptr<Parser> p = Parser::new_parser(l);
    shared_ptr<Program> program = p->parse_program();
    if (check_parser_errors(p)) {
      return;
    }

    try {
      define_macros(program, macro_env);
      auto expanded = expand_macros(program, macro_env);
      auto evaluated = eval::eval(expanded, env);
      if (evaluated != nullptr) {
        cout << evaluated->inspect() << endl;
      }
    } catch (std::runtime_error &e) {
      cout << "runtime error: " << e.what () << endl;
    }
  }

  auto load(const string &path, shared_ptr<Environment> env, shared_ptr<Environment> macro_env) -> void {
    ifstream in(path);
    string input((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    interp(input, env, macro_env);
  }

  auto run(const string &path) -> void {
    auto env = make_shared<Environment>();
    auto macro_env = make_shared<Environment>();
    load(path, env, macro_env);
  }
}
