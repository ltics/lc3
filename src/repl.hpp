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

      shared_ptr<lexer::Lexer> l = lexer::new_lexer(input_s);
      auto tok = lexer::next_token(l);
      while (tok.type != token::EOFT) {
        cout << tok.type << " - " << tok.literal << endl;
        tok = lexer::next_token(l);
      }

      free(input);
    }
  }
}

