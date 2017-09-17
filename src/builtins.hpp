#include "object.hpp"
#include "fmt/format.h"
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <range/v3/all.hpp>

using namespace std;
using namespace fmt;
using namespace ranges;
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
      return make_shared<Error>(format("argument to `len` not supported, got {0}", o->type()));
    }
  }

  auto puts_func(vector<shared_ptr<Object>> args) -> shared_ptr<Object> {
    std::for_each(args.cbegin(), args.cend(), [](shared_ptr<Object> o) {
        cout << o->inspect() << endl;
      });

    return make_shared<Null>();
  }

  auto first_func(vector<shared_ptr<Object>> args) -> shared_ptr<Object> {
    if (args.size() != 1) {
      string msg = format("wrong number of arguments. got={0}, want=1", args.size());
      return make_shared<Error>(msg);
    }

    shared_ptr<Object> o = args[0];
    if (o->type() != ARRAY_OBJ) {
      return make_shared<Error>(format("argument to `first` must be ARRAY, got {0}", o->type()));
    }

    shared_ptr<Array> arr = static_pointer_cast<Array>(o);
    if (arr->elements.size() > 0) {
      return arr->elements[0];
    } else {
      return make_shared<Null>();
    }
  }

  auto last_func(vector<shared_ptr<Object>> args) -> shared_ptr<Object> {
    if (args.size() != 1) {
      string msg = format("wrong number of arguments. got={0}, want=1", args.size());
      return make_shared<Error>(msg);
    }

    shared_ptr<Object> o = args[0];
    if (o->type() != ARRAY_OBJ) {
      return make_shared<Error>(format("argument to `last` must be ARRAY, got {0}", o->type()));
    }

    shared_ptr<Array> arr = static_pointer_cast<Array>(o);
    auto length = arr->elements.size();
    if (length > 0) {
      return arr->elements[length - 1];
    } else {
      return make_shared<Null>();
    }
  }

  auto rest_func(vector<shared_ptr<Object>> args) -> shared_ptr<Object> {
    if (args.size() != 1) {
      string msg = format("wrong number of arguments. got={0}, want=1", args.size());
      return make_shared<Error>(msg);
    }

    shared_ptr<Object> o = args[0];
    if (o->type() != ARRAY_OBJ) {
      return make_shared<Error>(format("argument to `rest` must be ARRAY, got {0}", o->type()));
    }

    shared_ptr<Array> arr = static_pointer_cast<Array>(o);
    auto length = arr->elements.size();
    if (length > 0) {
      vector<shared_ptr<Object>> rest = arr->elements | view::tail;
      return make_shared<Array>(rest);
    } else {
      return make_shared<Null>();
    }
  }

  auto push_func(vector<shared_ptr<Object>> args) -> shared_ptr<Object> {
    if (args.size() != 2) {
      string msg = format("wrong number of arguments. got={0}, want=2", args.size());
      return make_shared<Error>(msg);
    }

    shared_ptr<Object> o = args[0];
    if (o->type() != ARRAY_OBJ) {
      return make_shared<Error>(format("argument to `push` must be ARRAY, got {0}", o->type()));
    }

    shared_ptr<Array> arr = static_pointer_cast<Array>(o);
    auto elements = arr->elements;
    elements.push_back(args[1]);
    return make_shared<Array>(elements);
  }

  map<string, shared_ptr<Builtin>> builtins = {
    { "let", make_shared<Builtin>(let_func) },
    { "puts", make_shared<Builtin>(puts_func) },
    { "first", make_shared<Builtin>(first_func) },
    { "last", make_shared<Builtin>(last_func) },
    { "rest", make_shared<Builtin>(rest_func) },
    { "push", make_shared<Builtin>(push_func) }
  };
}
