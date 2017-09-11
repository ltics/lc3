#pragma once

#include <iostream>
#include <stdlib.h>
#include <editline/readline.h>
#include "lexer.hpp"

using namespace std;

namespace repl {
  void start() {
    cout << "lc3 Version 0.1" << endl;
    cout << "Press Ctrl+c to Exit\n" << endl;

    while (1) {
      char* input = readline("lc3> ");
      add_history(input);
      string input_s(input);

      shared_ptr<lexer::Lexer> l = lexer::Lexer::new_lexer(input_s);
      auto tok = l->next_token();
      while (tok.type != token::EOFT) {
        cout << tok.type << " - " << tok.literal << endl;
        tok = l->next_token();
      }

      free(input);
    }
  }
}

