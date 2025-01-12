#include "generator.h"
#include "template.h"
#include <cassert>
#include <fstream>
#include <iostream>

generator::Value generator::Scope::get(const std::string &name) {
  if (_symbols.contains(name))
    return _symbols[name];
  if (_parent)
    return _parent->get(name);
  throw std::runtime_error("Symbol not found: " + name);
}

void generator::Scope::set(const std::string &name, const Value &value,
                           const bool top) {
  if (top && _parent)
    _parent->set(name, value, true);
  else if (!_symbols.contains(name))
    _symbols[name] = value;
  else
    throw std::runtime_error("Symbol already defined: " + name);
}

std::string generator::Generator::generate() {
  _scope = std::make_shared<Scope>(nullptr);
  generateProgram(_ast);
  auto content = code_template;
  size_t pos;
  while ((pos = content.find("$1")) != std::string::npos) {
    content.replace(pos, 2, _header);
  }
  while ((pos = content.find("$2")) != std::string::npos) {
    content.replace(pos, 2, _body);
  }
  return content;
}

void generator::Generator::generateProgram(
    const std::shared_ptr<parser::ast::ASTNode> &ast) {
  assert(_ast->getType() == parser::ast::NodeType::Program);
  auto program = std::static_pointer_cast<parser::ast::ProgramNode>(ast);
  for (const auto &expression : program->getExpressions()) {
    generateExpression(expression, false);
    _body += ",";
  }
}

void generator::Generator::generateExpression(
    const std::shared_ptr<parser::ast::ASTNode> &ast, const bool quoted) {
  switch (const auto expression =
              std::static_pointer_cast<parser::ast::ExpressionNode>(ast);
          expression->getType()) {
  case parser::ast::NodeType::Integer:
  case parser::ast::NodeType::Floating:
  case parser::ast::NodeType::String:
    generateLiteral(ast);
    break;
  case parser::ast::NodeType::Identifier:
    generateIdentifier(ast, quoted);
    break;
  case parser::ast::NodeType::Quoted:
    generateQuoted(ast, quoted);
    break;
  case parser::ast::NodeType::Keyword:
    generateKeyword(ast, quoted);
    break;
  case parser::ast::NodeType::List:
    generateList(ast, quoted);
    break;
  default:
    throw std::runtime_error("Unexpected node type");
  }
}

void generator::Generator::generateLiteral(
    const std::shared_ptr<parser::ast::ASTNode> &ast) {
  assert(ast->getType() == parser::ast::NodeType::Integer ||
         ast->getType() == parser::ast::NodeType::Floating ||
         ast->getType() == parser::ast::NodeType::String);
  switch (ast->getType()) {
  case parser::ast::NodeType::Integer: {
    _body += "INT(";
    const auto node = std::static_pointer_cast<parser::ast::IntegerNode>(ast);
    _body += std::to_string(node->getValue());
    _body += ")";
    break;
  }
  case parser::ast::NodeType::Floating: {
    _body += "FLOAT(";
    const auto node = std::static_pointer_cast<parser::ast::FloatingNode>(ast);
    _body += std::to_string(node->getValue());
    _body += ")";
    break;
  }
  case parser::ast::NodeType::String: {
    _body += "STRING(\"";
    const auto node = std::static_pointer_cast<parser::ast::StringNode>(ast);
    for (const auto &c : node->getValue())
      if (c == '"')
        _body += "\\\"";
      else
        _body += c;
    _body += "\")";
    break;
  }
  default: {
    throw std::runtime_error("Unexpected node type");
  }
  }
}

void generator::Generator::generateIdentifier(
    const std::shared_ptr<parser::ast::ASTNode> &ast, const bool quoted) {
  assert(ast->getType() == parser::ast::NodeType::Identifier);
  const auto node = std::static_pointer_cast<parser::ast::IdentifierNode>(ast);
  if (quoted) {
    _body += "SYMBOL(\"";
    _body += node->getValue();
    _body += "\")";
  } else {
    Value value = _scope->get(node->getValue());
    if (value._type == ValueType::Function)
      throw std::runtime_error("Unexpected function");
    _body += value._value;
  }
}

void generator::Generator::generateKeyword(
    const std::shared_ptr<parser::ast::ASTNode> &ast, const bool quoted) {
  assert(ast->getType() == parser::ast::NodeType::Keyword);
  const auto node = std::static_pointer_cast<parser::ast::KeywordNode>(ast);
  if (quoted) {
    _body += "SYMBOL(\"";
    _body += node->getValue();
    _body += "\")";
  } else {
    if (node->getValue() == "nil") {
      _body += "NIL()";
    } else if (node->getValue() == "t") {
      _body += "T()";
    } else {
      throw std::runtime_error("Unexpected keyword");
    }
  }
}

void generator::Generator::generateQuoted(
    const std::shared_ptr<parser::ast::ASTNode> &ast, const bool quoted) {
  assert(ast->getType() == parser::ast::NodeType::Quoted);
  const auto node = std::static_pointer_cast<parser::ast::QuotedNode>(ast);
  if (quoted) {
    _body += "QUOTED(";
    generateExpression(node->getExpression(), true);
    _body += ")";
  } else {
    generateExpression(node->getExpression(), true);
  }
}

void generator::Generator::generateList(
    const std::shared_ptr<parser::ast::ASTNode> &ast, const bool quoted) {
  assert(ast->getType() == parser::ast::NodeType::List);
  const auto node = std::static_pointer_cast<parser::ast::ListNode>(ast);
  if (quoted) {
    _body += "LIST(";
    for (const auto &expr : node->getExpressions()) {
      generateExpression(expr, true);
      _body += ",";
    }
    _body += ")";
  } else {
    // TODO : function call
    const std::unordered_map<std::string, std::string> predefined = {
        {"null", "Null"},       {"not", "Not"},      {"if", "If"},
        {"car", "Car"},         {"cdr", "Cdr"},      {"cons", "Cons"},
        {"list", "List_"},      {"progn", "Progn"},  {"print", "Print"},
        {">=", "GreaterEqual"}, {"<=", "LessEqual"}, {">", "Greater"},
        {"<", "Less"},          {"=", "Equal"},      {"/=", "NotEqual"},
        {"+", "Add"},           {"-", "Subtract"},   {"*", "Multiply"},
        {"/", "Divide"},
    };
    const auto first = node->getExpressions().front();
    const auto rest = std::vector(node->getExpressions().begin() + 1,
                                  node->getExpressions().end());
    if (first->getType() == parser::ast::NodeType::Identifier) {
      const auto ident =
          std::static_pointer_cast<parser::ast::IdentifierNode>(first);
      if (const Value value = _scope->get(ident->getValue());
          value._type == ValueType::Function) {
        generateFunctionCall(value._value, rest);
      } else {
        throw std::runtime_error("Unexpected function");
      }
    } else if (first->getType() == parser::ast::NodeType::Keyword) {
      if (const auto keyword =
              std::static_pointer_cast<parser::ast::KeywordNode>(first);
          predefined.contains(keyword->getValue())) {
        generateFunctionCall(predefined.at(keyword->getValue()), rest);
      } else if (keyword->getValue() == "defun" && rest.size() == 3) {
        generateDefun(rest[0], rest[1], rest[2]);
      } else if (keyword->getValue() == "let" && rest.size() == 2) {
        generateLet(rest[0], rest[1]);
      } else {
        throw std::runtime_error("Unexpected keyword");
      }
    } else {
      throw std::runtime_error("Unexpected function call");
    }
  }
}

void generator::Generator::generateFunctionCall(
    const std::string &name,
    const std::vector<std::shared_ptr<parser::ast::ASTNode>> &args) {
  _body += "FUNC(";
  _body += name;
  _body += ", ";
  for (const auto &expr : args) {
    generateExpression(expr, false);
    _body += ",";
  }
  _body += ")";
}

// (defun ident (ident1 ident2) (expression))
void generator::Generator::generateDefun(
    const std::shared_ptr<parser::ast::ASTNode> &name,
    const std::shared_ptr<parser::ast::ASTNode> &args,
    const std::shared_ptr<parser::ast::ASTNode> &body) {

  auto original_scope = _scope;
  _scope = std::make_shared<Scope>(original_scope);

  static unsigned int func_count = 0;
  const std::string generated_name = "L" + std::to_string(func_count);
  func_count++;
  std::string func_name;
  int args_count = 0;
  std::string body_str;

  // 1. Generate function name
  {
    if (name->getType() != parser::ast::NodeType::Identifier)
      throw std::runtime_error("Invalid function name");
    auto ident = std::static_pointer_cast<parser::ast::IdentifierNode>(name);
    func_name = ident->getValue();
  }

  // Set function to top level scope
  _scope->set(func_name, Value(ValueType::Function, generated_name), true);

  // 2. Generate arguments
  {
    if (args->getType() == parser::ast::NodeType::List)
      for (const auto list =
               std::static_pointer_cast<parser::ast::ListNode>(args);
           const auto &expr : list->getExpressions()) {
        if (expr->getType() != parser::ast::NodeType::Identifier)
          throw std::runtime_error("Invalid argument");
        const auto ident =
            std::static_pointer_cast<parser::ast::IdentifierNode>(expr);
        _scope->set(ident->getValue(),
                    Value(ValueType::Expression,
                          "ARG(" + std::to_string(args_count) + ")"));
        args_count++;
      }
    else if (args->getType() == parser::ast::NodeType::Keyword) {
      if (const auto keyword =
              std::static_pointer_cast<parser::ast::KeywordNode>(args);
          keyword->getValue() != "nil")
        throw std::runtime_error("Invalid arguments list");
    } else
      throw std::runtime_error("Invalid arguments list");
  }

  // 3. Generate body
  {
    const std::size_t pos = _body.size();
    generateExpression(body, false);
    body_str = _body.substr(pos);
    _body.resize(pos);
  }

  _header += "DEF(";
  _header += generated_name;
  _header += ",";
  _header += std::to_string(args_count);
  _header += ",";
  _header += body_str;
  _header += ");\n";

  _body += "SYMBOL(\"";
  _body += func_name;
  _body += "\")";

  _scope = original_scope;
}

void generator::Generator::generateLet(
    const std::shared_ptr<parser::ast::ASTNode> &assignments,
    const std::shared_ptr<parser::ast::ASTNode> &body) {
  auto original_scope = _scope;
  _scope = std::make_shared<Scope>(original_scope);

  // 1. assign variables to current scope
  // format : ( (ident1 expr1) (ident2 expr2) ... )
  {
    if (assignments->getType() != parser::ast::NodeType::List)
      throw std::runtime_error("Invalid assignments list");
    for (const auto list =
             std::static_pointer_cast<parser::ast::ListNode>(assignments);
         const auto &expr : list->getExpressions()) {
      if (expr->getType() != parser::ast::NodeType::List)
        throw std::runtime_error("Invalid assignment");
      const auto assignment =
          std::static_pointer_cast<parser::ast::ListNode>(expr);
      if (assignment->getExpressions().size() != 2 &&
          assignment->getExpressions().front()->getType() !=
              parser::ast::NodeType::Identifier)
        throw std::runtime_error("Invalid assignment");
      const auto ident = std::static_pointer_cast<parser::ast::IdentifierNode>(
          assignment->getExpressions().front());
      const auto value = assignment->getExpressions().back();
      const std::size_t pos = _body.size();
      generateExpression(value, false);
      _scope->set(ident->getValue(),
                  Value(ValueType::Expression, _body.substr(pos)));
      _body.resize(pos);
    }
  }

  // 2. generate body
  {
    generateExpression(body, false);
  }

  _scope = original_scope;
}
