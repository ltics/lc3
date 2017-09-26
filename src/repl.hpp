#pragma once

#include <iostream>
#include <stdlib.h>
#include <editline/readline.h>
#include "object.hpp"
#include "interpret.hpp"

using namespace std;
using namespace object;
using namespace interpret;

namespace repl {
  void start() {
    cout << "lc3 Version 0.1" << endl;
    cout << "Press Ctrl+c to Exit\n" << endl;

    auto env = make_shared<Environment>();
    auto macro_env = make_shared<Environment>();

    load("./lib/std.lc3", env, macro_env);

    while (1) {
      char* input = readline("lc3> ");
      add_history(input);
      string input_s(input);

      interp(input_s, env, macro_env);

      free(input);
    }
  }
}

