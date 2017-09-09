#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <string>

using namespace std;

namespace testutil {
  struct TestVariant {
    enum { t_string, t_int, t_bool } type_id;
    union {
      int as_int;
      bool as_bool;
      string as_string;
    };

    TestVariant() : type_id{t_int}, as_int{0} {}
    TestVariant(int v) : type_id{t_int}, as_int{v} {}
    TestVariant(bool v) : type_id{t_bool}, as_bool{v} {}
    TestVariant(string v) : type_id{t_string}, as_string{v} {}
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
          throw std::runtime_error("unknown type");
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
  };
}

