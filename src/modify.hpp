#pragma once

#include "ast.hpp"
#include <map>
#include <memory>
#include <functional>
#include <range/v3/all.hpp>

using namespace std;
using namespace ast;
using namespace ranges;

namespace modify {
  typedef std::function<shared_ptr<Node>(shared_ptr<Node>)> modifier_func;

  auto modify(shared_ptr<Node> node, modifier_func modifier) -> shared_ptr<Node> {
    shared_ptr<Node> modified;
    switch (node->type()) {
    case NodeType::PROGRAM: {
      auto program = static_pointer_cast<Program>(node);
      auto new_statements = program->statements | view::transform([&](shared_ptr<Statement> stmt) {
          return static_pointer_cast<Statement>(modify(stmt, modifier));
        });
      modified = make_shared<Program>(new_statements);
      break;
    }
    case NodeType::EXPRESSIONSTATEMENT: {
      auto expr_stmt = static_pointer_cast<ExpressionStatement>(node);
      auto new_expr = static_pointer_cast<Expression>(modify(expr_stmt->expression, modifier));
      modified = make_shared<ExpressionStatement>(expr_stmt->token, new_expr);
      break;
    }
    case NodeType::INFIXEXPRESSION: {
      auto infix = static_pointer_cast<InfixExpression>(node);
      auto new_left = static_pointer_cast<Expression>(modify(infix->left, modifier));
      auto new_right = static_pointer_cast<Expression>(modify(infix->right, modifier));
      modified = make_shared<InfixExpression>(infix->token, new_left, infix->infix_operator, new_right);
      break;
    }
    case NodeType::PREFIXEXPRESSION: {
      auto prefix = static_pointer_cast<PrefixExpression>(node);
      auto new_right = static_pointer_cast<Expression>(modify(prefix->right, modifier));
      modified = make_shared<PrefixExpression>(prefix->token, prefix->prefix_operator, new_right);
      break;
    }
    case NodeType::INDEXEXPRESSION: {
      auto index_expr = static_pointer_cast<IndexExpression>(node);
      auto new_left = static_pointer_cast<Expression>(modify(index_expr->left, modifier));
      auto new_index = static_pointer_cast<Expression>(modify(index_expr->index, modifier));
      modified = make_shared<IndexExpression>(index_expr->token, new_left, new_index);
      break;
    }
    case NodeType::IFEXPRESSION: {
      auto if_expr = static_pointer_cast<IfExpression>(node);
      auto new_condition = static_pointer_cast<Expression>(modify(if_expr->condition, modifier));
      auto new_consequence = static_pointer_cast<BlockStatement>(modify(if_expr->consequence, modifier));
      shared_ptr<BlockStatement> new_alternative;
      if (if_expr->alternative != nullptr) {
        new_alternative = static_pointer_cast<BlockStatement>(modify(if_expr->alternative, modifier));
      }
      modified = make_shared<IfExpression>(if_expr->token, new_condition, new_consequence, new_alternative);
      break;
    }
    case NodeType::BLOCKSTATEMENT: {
      auto block = static_pointer_cast<BlockStatement>(node);
      auto new_statements = block->statements | view::transform([&](shared_ptr<Statement> stmt) {
          return static_pointer_cast<Statement>(modify(stmt, modifier));
        });
      modified = make_shared<BlockStatement>(block->token, new_statements);
      break;
    }
    case NodeType::RETURNSTATEMENT: {
      auto return_expr = static_pointer_cast<ReturnStatement>(node);
      auto new_value = static_pointer_cast<Expression>(modify(return_expr->value, modifier));
      modified = make_shared<ReturnStatement>(return_expr->token, new_value);
      break;
    }
    case NodeType::LETSTATEMENT: {
      auto let_expr = static_pointer_cast<LetStatement>(node);
      auto new_value = static_pointer_cast<Expression>(modify(let_expr->value, modifier));
      modified = make_shared<LetStatement>(let_expr->token, let_expr->name, new_value);
      break;
    }
    case NodeType::FUNCTIONLITERAL: {
      auto func = static_pointer_cast<FunctionLiteral>(node);
      auto new_parameters = func->parameters | view::transform([&](shared_ptr<Identifier> param) {
          return static_pointer_cast<Identifier>(modify(param, modifier));
        });
      auto new_body = static_pointer_cast<BlockStatement>(modify(func->body, modifier));
      modified = make_shared<FunctionLiteral>(func->token, new_parameters, new_body);
      break;
    }
    case NodeType::ARRAYLITERAL: {
      auto arr = static_pointer_cast<ArrayLiteral>(node);
      auto new_elements = arr->elements | view::transform([&](shared_ptr<Expression> elem) {
          return static_pointer_cast<Expression>(modify(elem, modifier));
        });
      modified = make_shared<ArrayLiteral>(arr->token, new_elements);
      break;
    }
    case NodeType::HASHLITERAL: {
      auto hash = static_pointer_cast<HashLiteral>(node);
      map<shared_ptr<ast::Expression>, shared_ptr<ast::Expression>> new_pairs = {};
      for (auto it = hash->pairs.begin(); it != hash->pairs.end(); it++) {
        auto new_key = static_pointer_cast<Expression>(modify(it->first, modifier));
        auto new_value = static_pointer_cast<Expression>(modify(it->second, modifier));
        new_pairs[new_key] = new_value;
      }
      modified = make_shared<HashLiteral>(hash->token, new_pairs);
      break;
    }
    default:
      modified = node;
      break;
    }
    return modifier(modified);
  }
}
