#pragma once

#include "ast.hpp"
#include "object.hpp"
#include "modify.hpp"
#include "eval.hpp"
#include <vector>
#include <memory>
#include <range/v3/all.hpp>

using namespace std;
using namespace ast;
using namespace object;
using namespace ranges;
using namespace modify;
using namespace eval;

namespace macroexpansion {
  auto is_macro_definition(shared_ptr<Node> node) -> bool {
    if (node->type() != NodeType::LETSTATEMENT) {
      return false;
    }

    auto let = static_pointer_cast<LetStatement>(node);

    if (let->value->type() != NodeType::MACROLITERAL) {
      return false;
    }

    return true;
  }

  auto is_macro_call(shared_ptr<CallExpression> expr, shared_ptr<Environment> env) -> shared_ptr<Macro> {
    if (expr->function->type() != NodeType::IDENTIFIER) {
      return nullptr;
    }

    auto obj = env->get(static_pointer_cast<Identifier>(expr->function)->value);
    if (obj == nullptr) {
      return nullptr;
    }

    if (obj->type() != MACRO_OBJ) {
      return nullptr;
    }

    return static_pointer_cast<Macro>(obj);
  }

  auto add_macro(shared_ptr<Statement> stmt, shared_ptr<Environment> env) -> void {
    auto let = static_pointer_cast<LetStatement>(stmt);
    auto macro = static_pointer_cast<MacroLiteral>(let->value);

    auto macro_obj = make_shared<Macro>(macro->parameters, macro->body, env);
    env->set(let->name->value, macro_obj);
  }

  auto quote_args(shared_ptr<CallExpression> expr) -> vector<shared_ptr<Quote>> {
    return expr->arguments | view::transform([](shared_ptr<Expression> arg) {
        return make_shared<Quote>(arg);
      });
  }

  auto extend_macro_env(shared_ptr<Macro> macro, vector<shared_ptr<Quote>> args) -> shared_ptr<Environment> {
    auto extended = new_enclosed_environment(macro->env);

    for (size_t i = 0; i < macro->parameters.size(); i++) {
      extended->set(macro->parameters[i]->value, args[i]);
    }

    return extended;
  }

  auto define_macros(shared_ptr<Program> program, shared_ptr<Environment> env) -> void {
    vector<shared_ptr<Statement>> new_statements = {};
    auto stmts = program->statements;
    std::for_each(stmts.cbegin(), stmts.cend(), [&](shared_ptr<Statement> stmt) {
        if (is_macro_definition(stmt)) {
          add_macro(stmt, env);
        } else {
          new_statements.push_back(stmt);
        }
      });
    program->statements = new_statements;
  }

  auto expand_macros(shared_ptr<Node> program, shared_ptr<Environment> env) -> shared_ptr<Node> {
    return modify::modify(program, [&](shared_ptr<Node> node) -> shared_ptr<Node> {
        if (node->type() != NodeType::CALLEXPRESSION) {
          return node;
        }

        auto call_expr = static_pointer_cast<CallExpression>(node);

        auto macro = is_macro_call(call_expr, env);
        if (macro == nullptr) {
          return node;
        }

        auto args = quote_args(call_expr);
        auto eval_env = extend_macro_env(macro, args);
        auto evaluated = eval::eval(macro->body, eval_env);

        if (evaluated->type() != QUOTE_OBJ) {
          throw std::runtime_error("we only support returning AST-nodes from macros");
        }

        return static_pointer_cast<Quote>(evaluated)->node;
      });
  }
}
