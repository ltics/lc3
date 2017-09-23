#pragma once

#include "ast.hpp"
#include "token.hpp"
#include "object.hpp"
#include <memory>
#ifndef FORMAT_HEADER
#define FORMAT_HEADER
#include <fmt/format.h>
#include <fmt/format.cc>
#endif

using namespace std;
using namespace ast;
using namespace fmt;
using namespace token;
using namespace object;

namespace quoteunquote {
  auto convert_object_to_node(shared_ptr<Object> obj) -> shared_ptr<Node> {
    if (obj->type() == INTEGER_OBJ) {
      auto int_obj = static_pointer_cast<Integer>(obj);
      Token t = { INT, format("{}", int_obj->value) };
      return make_shared<IntegerLiteral>(t, int_obj->value);
    } else if (obj->type() == BOOLEAN_OBJ) {
      auto bool_obj = static_pointer_cast<object::Boolean>(obj);
      Token t;
      if (bool_obj->value) {
        t = { TRUET, "true" };
      } else {
        t = { FALSET, "false" };
      }
      return make_shared<ast::Boolean>(t, bool_obj->value);
    } else if (obj->type() == QUOTE_OBJ) {
      return static_pointer_cast<Quote>(obj)->node;
    } else {
      return nullptr;
    }
  }

  auto is_unquote_call(shared_ptr<Node> node) -> bool {
    if (node->type() != NodeType::CALLEXPRESSION) {
      return false;
    }
    auto call_expr = static_pointer_cast<CallExpression>(node);
    return call_expr->function->token_literal() == "unquote";
  }
}
