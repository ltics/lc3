#include "object.hpp"
#include "fmt/format.h"
#include <map>
#include <string>
#include <vector>
#include <iostream>

using namespace std;
using namespace fmt;
using namespace object;

namespace builtins {
  auto let_func(vector<shared_ptr<Object>> args) -> shared_ptr<Object> {
    if (args.size() != 1) {
      string msg = format("wrong number of arguments. got={0}, want=1", args.size());
      return make_shared<Error>(msg);
    }

    shared_ptr<Object> o = args[0];
    if (o->type() == ARRAY_OBJ) {
      shared_ptr<Array> arr = static_pointer_cast<Array>(o);
      return make_shared<Integer>(arr->elements.size());
    } else if (o->type() == STRING_OBJ) {
      shared_ptr<String> str = static_pointer_cast<String>(o);
      return make_shared<Integer>(str->value.size());
    } else {
      return make_shared<Error>(format("argument to 'len' not supported, got {0}", o->type()));
    }
  }

  map<string, shared_ptr<Builtin>> builtins = {
    { "let", make_shared<Builtin>(let_func) }
  };
}
