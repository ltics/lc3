#pragma once

#include "../src/lexer.hpp"
#include "../src/parser.hpp"
#include "../src/eval.hpp"
#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <string>

using namespace std;
using namespace lexer;
using namespace parser;
using namespace eval;

namespace testutil {
  auto test_eval(string input) -> shared_ptr<Object> {
    auto lexer = Lexer::new_lexer(input);
    auto parser = Parser::new_parser(lexer);
    auto program = parser->parse_program();
    auto env = make_shared<Environment>();

    return eval::eval(program, env);
  }

  struct TestVariant {
    enum { t_string, t_int, t_bool } type_id;
    union {
      int as_int;
      bool as_bool;
      string as_string;
    };

    explicit TestVariant(int v) : type_id{t_int}, as_int{v} {}
    explicit TestVariant(bool v) : type_id{t_bool}, as_bool{v} {}
    explicit TestVariant(const string &v) : type_id{t_string}, as_string{v} {}
    ~TestVariant()
    {
      switch(type_id)
        {
        case t_int:
        case t_bool:
          // trivially destructible, no need to do anything
          break;
        case t_string:
          as_string.~basic_string();
          break;
        default:
          //throw std::runtime_error("unknown type");
          break;
        }
    }

    TestVariant(const TestVariant& other)
    {
      type_id = other.type_id;
      switch (type_id) {
      case t_string:
        new (&as_string) auto(other.as_string);
        break;
      case t_int:
        as_int = other.as_int;
        break;
      case t_bool:
        as_bool = other.as_bool;
        break;
      default:
        throw std::runtime_error("unknown type");
      }
    }

    TestVariant &operator=(const TestVariant &other) {
      if (this->type_id == other.type_id) {
        switch (this->type_id) {
        case t_string:
          this->as_string = other.as_string;
          break;
        case t_int:
          this->as_int = other.as_int;
          break;
        case t_bool:
          this->as_bool = other.as_bool;
          break;
        default:
          throw std::runtime_error("unknown type");
        }
      }

      return *this;
    }
  };
}

