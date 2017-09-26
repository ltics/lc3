#include "repl.hpp"
#include "interpret.hpp"
#include <string>

using namespace std;

int main(int argc, char** argv) {
  if (argc == 2) {
    string arg(argv[1]);
    if (arg == "repl") {
      repl::start();
    } else {
      interpret::run(arg);
    }
  } else {
    repl::start();
  }
  return 0;
}
