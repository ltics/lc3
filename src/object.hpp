#pragma once

#include "ast.hpp"
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <iostream>
#include <functional>
#include <range/v3/all.hpp>

using namespace std;
using namespace ast;
using namespace ranges;

namespace object {
  auto flatten_strings(vector<string> strs) -> string {
    if (strs.size() == 0) {
      return "";
    } else if (strs.size() == 1) {
      return strs[0];
    } else {
      vector<string> literals = strs | view::take(strs.size() - 1) | view::transform([](string s) {
          s += ",";
          return s;
        });
      literals.push_back(strs[strs.size() - 1]);
      return literals | view::join(' ');
    }
  }

  typedef string ObjectType;

  const ObjectType NULL_OBJ  = "NULL";
  const ObjectType ERROR_OBJ = "ERROR";
  const ObjectType INTEGER_OBJ = "INTEGER";
  const ObjectType BOOLEAN_OBJ = "BOOLEAN";
  const ObjectType STRING_OBJ  = "STRING";
  const ObjectType RETURN_VALUE_OBJ = "RETURN_VALUE";
  const ObjectType FUNCTION_OBJ = "FUNCTION";
  const ObjectType BUILTIN_OBJ  = "BUILTIN";
  const ObjectType ARRAY_OBJ = "ARRAY";
  const ObjectType HASH_OBJ  = "HASH";

  class Object {
  public:
    virtual ObjectType type() = 0;
    virtual string inspect() = 0;
  };

  class Environment {
  public:
    map<string, shared_ptr<Object>> store = {};
    shared_ptr<Environment> outer = nullptr;

    shared_ptr<Environment> new_enclosed_environment(shared_ptr<Environment> outer) {
      shared_ptr<Environment> env = make_shared<Environment>();
      env->outer = outer;
      return env;
    }

    shared_ptr<Object> get(string name) {
      auto result = this->store.find(name);
      if (result == this->store.end() && this->outer != nullptr) {
        return this->outer->get(name);
      } else {
        return this->store[name];
      }
    }

    shared_ptr<Object> set(string name, shared_ptr<Object> value) {
      this->store[name] = value;
      return value;
    }
  };

  typedef function<shared_ptr<Object>(vector<shared_ptr<Object>> args)> BuiltinFunction;

  class HashKey {
  public:
    ObjectType type;
    unsigned int value;

    HashKey(ObjectType t, unsigned int v): type(t), value(v) {};
  };

  class Hashable {
  public:
    virtual HashKey hash_key() = 0;
  };

  class Integer : public Object, public Hashable {
  public:
    int value;

    Integer(int v): value(v) {};

    ObjectType type() {
      return INTEGER_OBJ;
    }

    string inspect() {
      stringstream ss;
      ss << this->value;
      return ss.str();
    }

    HashKey hash_key() {
      return HashKey(this->type(), this->value);
    }
  };

  class Boolean : public Object, public Hashable {
  public:
    bool value;

    Boolean(bool v): value(v) {};

    ObjectType type() {
      return BOOLEAN_OBJ;
    }

    string inspect() {
      stringstream ss;
      ss << this->value;
      return ss.str();
    }

    HashKey hash_key() {
      return HashKey(this->type(), this->value ? 1 : 0);
    }
  };

  class Null : public Object {
  public:
    ObjectType type() {
      return NULL_OBJ;
    }

    string inspect() {
      return "null";
    }
  };

  class ReturnValue : public Object {
  public:
    shared_ptr<Object> value;

    ReturnValue(shared_ptr<Object> v): value(v) {};

    ObjectType type() {
      return RETURN_VALUE_OBJ;
    }

    string inspect() {
      return this->value->inspect();
    }
  };

  class Error : public Object {
  public:
    string message;

    Error(string m): message(m) {};

    ObjectType type() {
      return ERROR_OBJ;
    }

    string inspect() {
      return "ERROR: " + this->message;
    }
  };

  class Function : public Object {
  public:
    vector<shared_ptr<Identifier>> parameters;
    shared_ptr<BlockStatement> body;
    shared_ptr<Environment> env;

    Function(vector<shared_ptr<Identifier>> ps,
             shared_ptr<BlockStatement> b,
             shared_ptr<Environment> e)
      : parameters(ps), body(b), env(e) {};

    ObjectType type() {
      return FUNCTION_OBJ;
    }

    string inspect() {
      string s("");
      string params("");
      std::for_each(parameters.cbegin(), parameters.cend(), [&](shared_ptr<Identifier> param) {
          params += param->to_string();
        });

      s += "fn(";
      s += params;
      s += ") {\n";
      s += this->body->to_string();
      s += "\n}";

      return s;
    }
  };

  class String : public Object {
  public:
    string value;

    String(string v): value(v) {};

    ObjectType type() {
      return STRING_OBJ;
    }

    string inspect() {
      return this->value;
    }
  };

  class Builtin : public Object {
  public:
    BuiltinFunction func;

    Builtin(BuiltinFunction f): func(f) {};

    ObjectType type() {
      return BUILTIN_OBJ;
    }

    string inspect() {
      return "builtin function";
    }
  };

  class Array : public Object {
  public:
    vector<shared_ptr<Object>> elements;

    Array(vector<shared_ptr<Object>> es): elements(es) {};

    ObjectType type() {
      return ARRAY_OBJ;
    }

    string inspect() {
      string s("");
      string elems("");
      std::for_each(elements.cbegin(), elements.cend(), [&](shared_ptr<Object> e) {
          elems += e->inspect();
        });

      s += "[";
      s += elems;
      s += "]";

      return s;
    }
  };

  class HashPair {
  public:
    shared_ptr<Object> key;
    shared_ptr<Object> value;

    HashPair(shared_ptr<Object> k,
             shared_ptr<Object> v)
      : key(k), value(v) {};
  };

  class Hash : public Object {
  public:
    map<HashKey, HashPair> pairs;

    Hash(map<HashKey, HashPair> ps): pairs(ps) {};

    ObjectType type() {
      return HASH_OBJ;
    }

    string inspect() {
      string s("");
      vector<string> pairs_strs = pairs | view::transform([](pair<HashKey, HashPair> p) {
          string pair_str;
          pair_str += p.second.key->inspect();
          pair_str += ": ";
          pair_str += p.second.key->inspect();
          return pair_str;
        });

      s += "{";
      s += flatten_strings(pairs_strs);
      s += "}";

      return s;
    }
  };
}
