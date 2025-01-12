#ifndef GENERATOR_H
#define GENERATOR_H

#include "ast.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

namespace generator {

enum class ValueType {
  Function,
  Expression,
};

struct Value {
  ~Value() = default;
  explicit Value() = default;
  explicit Value(const ValueType type, std::string value)
      : _type(type), _value(std::move(value)) {}

  ValueType _type{};
  std::string _value;
};

class Scope {
public:
  explicit Scope(const std::shared_ptr<Scope> &parent) : _parent(parent) {}
  Value get(const std::string &name);
  void set(const std::string &name, const Value &value, bool top = false);
  std::shared_ptr<Scope> getParent() { return _parent; }

private:
  std::shared_ptr<Scope> _parent;
  std::unordered_map<std::string, Value> _symbols;
};

class Generator {
public:
  explicit Generator(const std::shared_ptr<parser::ast::ASTNode> &ast)
      : _ast(ast), _scope(nullptr) {}
  std::string generate();

private:
  void generateProgram(const std::shared_ptr<parser::ast::ASTNode> &ast);
  void generateExpression(const std::shared_ptr<parser::ast::ASTNode> &ast,
                          bool quoted);
  void generateLiteral(const std::shared_ptr<parser::ast::ASTNode> &ast);
  void generateIdentifier(const std::shared_ptr<parser::ast::ASTNode> &ast,
                          bool quoted);
  void generateKeyword(const std::shared_ptr<parser::ast::ASTNode> &ast,
                       bool quoted);
  void generateQuoted(const std::shared_ptr<parser::ast::ASTNode> &ast,
                      bool quoted);
  void generateList(const std::shared_ptr<parser::ast::ASTNode> &ast,
                    bool quoted);
  void generateFunctionCall(
      const std::string &name,
      const std::vector<std::shared_ptr<parser::ast::ASTNode>> &args);
  void generateDefun(const std::shared_ptr<parser::ast::ASTNode> &name,
                     const std::shared_ptr<parser::ast::ASTNode> &args,
                     const std::shared_ptr<parser::ast::ASTNode> &body);
  void generateLet(const std::shared_ptr<parser::ast::ASTNode> &assignments,
                   const std::shared_ptr<parser::ast::ASTNode> &body);
  std::shared_ptr<parser::ast::ASTNode> _ast;
  std::shared_ptr<Scope> _scope;
  std::string _header;
  std::string _body;
};

} // namespace generator

#endif // GENERATOR_H