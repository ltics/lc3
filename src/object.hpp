#pragma once

#include "ast.hpp"
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include <sstream>
#include <iostream>
#include <functional>
#include <range/v3/all.hpp>
#ifndef FORMAT_HEADER
#define FORMAT_HEADER
#include <fmt/format.h>
#include <fmt/format.cc>
#endif

using namespace std;
using namespace ast;
using namespace fmt;
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
  typedef string HashKey;

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
  const ObjectType QUOTE_OBJ = "QUOTE";

  class Object {
  public:
    virtual ObjectType type() = 0;
    virtual string inspect() = 0;
  };

  class Environment {
  public:
    map<string, shared_ptr<Object>> store = {};
    shared_ptr<Environment> outer = nullptr;

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

  shared_ptr<Environment> new_enclosed_environment(shared_ptr<Environment> outer) {
    shared_ptr<Environment> env = make_shared<Environment>();
    env->outer = outer;
    return env;
  }

  typedef function<shared_ptr<Object>(vector<shared_ptr<Object>> args)> BuiltinFunction;

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
      stringstream ss;
      ss << this->type();
      ss << "-";
      ss << this->value;
      return ss.str();
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
      stringstream ss;
      ss << this->type();
      ss << "-";
      ss << (this->value ? 1 : 0);
      return ss.str();
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

  class String : public Object, public Hashable {
  public:
    string value;

    String(string v): value(v) {};

    ObjectType type() {
      return STRING_OBJ;
    }

    string inspect() {
      return this->value;
    }

    HashKey hash_key() {
      size_t value_hash = hash<string>{}(this->value);

      stringstream ss;
      ss << this->type();
      ss << "-";
      ss << value_hash;
      return ss.str();
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
      string elems = flatten_strings(elements | view::transform([](shared_ptr<Object> o) { return o->inspect(); }));

      s += "[";
      s += elems;
      s += "]";

      return s;
    }
  };

  typedef pair<shared_ptr<Object>, shared_ptr<Object>> HashPair;

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
          pair_str += p.second.first->inspect();
          pair_str += ": ";
          pair_str += p.second.second->inspect();
          return pair_str;
        });

      s += "{";
      s += flatten_strings(pairs_strs);
      s += "}";

      return s;
    }
  };

  class Quote : public Object {
  public:
    shared_ptr<Node> node;

    Quote(shared_ptr<Node> n): node(n) {};

    string type() {
      return QUOTE_OBJ;
    }

    string inspect() {
      return format("QUOTE({0})", this->node->to_string());
    }
  };

  bool operator==(shared_ptr<Object> obj1, shared_ptr<Object> obj2) {
    if (obj1->type() != obj2->type()) {
      return false;
    } else {
      if (obj1->type() == INTEGER_OBJ) {
        return static_pointer_cast<Integer>(obj1)->value == static_pointer_cast<Integer>(obj2)->value;
      } else if (obj1->type() == BOOLEAN_OBJ) {
        return static_pointer_cast<Boolean>(obj1)->value == static_pointer_cast<Boolean>(obj2)->value;
      } else if (obj1->type() == STRING_OBJ) {
        return static_pointer_cast<String>(obj1)->value == static_pointer_cast<String>(obj2)->value;
      } else if (obj1->type() == RETURN_VALUE_OBJ) {
        return static_pointer_cast<ReturnValue>(obj1)->value == static_pointer_cast<ReturnValue>(obj2)->value;
      } else if (obj1->type() == ARRAY_OBJ) {
        auto elems1 = static_pointer_cast<Array>(obj1)->elements;
        auto elems2 = static_pointer_cast<Array>(obj2)->elements;
        if (elems1.size() == elems2.size()) {
          for (int i = 0; i < elems1.size(); i++) {
            if (!(elems1[i] == elems2[i])) {
              return false;
            }
          }
          return true;
        } else {
          return false;
        }
      } else if (obj1->type() == HASH_OBJ) {
        auto pairs1 = static_pointer_cast<Hash>(obj1)->pairs;
        auto pairs2 = static_pointer_cast<Hash>(obj2)->pairs;
        if (pairs1.size() == pairs2.size()) {
          for (auto entry1 = pairs1.begin(),
                    entry2 = pairs2.begin();
               entry1 != pairs1.end();
               ++entry1 , ++entry2) {
            if (!(entry1->first == entry2->first &&
                  entry1->second.first == entry2->second.first &&
                  entry1->second.second == entry2->second.second)) {
              return false;
            }
          }
          return true;
        } else {
          return false;
        }
      } else if (obj1->type() == ERROR_OBJ) {
        return static_pointer_cast<Error>(obj1)->message == static_pointer_cast<Error>(obj2)->message;
      } else if (obj1->type() == NULL_OBJ) {
        return true;
      } else if (obj1->type() == QUOTE_OBJ) {
        return static_pointer_cast<Quote>(obj1)->node->to_string() == static_pointer_cast<Quote>(obj2)->node->to_string();
      } else {
        return false;
      }
    }
  }

  bool operator!=(shared_ptr<Object> obj1, shared_ptr<Object> obj2) {
    return !(obj1 == obj2);
  }

  bool is_hashable(shared_ptr<Object> obj) {
    return
      obj->type() == INTEGER_OBJ ||
      obj->type() == BOOLEAN_OBJ ||
      obj->type() == STRING_OBJ;
  }
}
