#pragma once

#include <iostream>
#include <stdlib.h>
#include <editline/readline.h>
#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "object.hpp"
#include "eval.hpp"

using namespace std;
using namespace ast;
using namespace lexer;
using namespace parser;
using namespace object;
using namespace eval;

namespace repl {
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

  void start() {
    cout << "lc3 Version 0.1" << endl;
    cout << "Press Ctrl+c to Exit\n" << endl;

    while (1) {
      char* input = readline("lc3> ");
      add_history(input);
      string input_s(input);

      shared_ptr<Lexer> l = Lexer::new_lexer(input_s);
      shared_ptr<Parser> p = Parser::new_parser(l);
      shared_ptr<Program> program = p->parse_program();
      if (check_parser_errors(p)) {
        continue;
      }

      auto env = make_shared<Environment>();
      auto evaluated = eval::eval(program, env);
      if (evaluated != nullptr) {
        cout << evaluated->inspect() << endl;
      }

      free(input);
    }
  }
}

