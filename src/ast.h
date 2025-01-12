#ifndef AST_H
#define AST_H
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace parser::ast {

enum class NodeType {
  Program,
  Integer,
  Floating,
  String,
  List,
  Quoted,
  Keyword,
  Identifier,
};

class ASTNode {
public:
  explicit ASTNode(const NodeType type) : _type(type) {}
  [[nodiscard]] NodeType getType() const { return _type; }

private:
  NodeType _type;
};

class ProgramNode final : public ASTNode {
public:
  explicit ProgramNode() : ASTNode(NodeType::Program) {}
  void addExpression(const std::shared_ptr<ASTNode> &expression) {
    _expressions.push_back(expression);
  }
  [[nodiscard]] const std::vector<std::shared_ptr<ASTNode>> &
  getExpressions() const {
    return _expressions;
  }

private:
  std::vector<std::shared_ptr<ASTNode>> _expressions;
};

class ExpressionNode : public ASTNode {
public:
  explicit ExpressionNode(const NodeType type) : ASTNode(type) {}
};

class IntegerNode final : public ExpressionNode {
public:
  explicit IntegerNode(const int value)
      : ExpressionNode(NodeType::Integer), _value(value) {}
  [[nodiscard]] int getValue() const { return _value; }
private:
  int _value;
};

class FloatingNode final : public ExpressionNode {
public:
  explicit FloatingNode(const double value)
      : ExpressionNode(NodeType::Floating), _value(value) {}
  [[nodiscard]] double getValue() const { return _value; }

private:
  double _value;
};

class StringNode final : public ExpressionNode {
public:
  explicit StringNode(std::string value)
      : ExpressionNode(NodeType::String), _value(std::move(value)) {}
  [[nodiscard]] const std::string &getValue() const { return _value; }

private:
  std::string _value;
};

class ListNode final : public ExpressionNode {
public:
  explicit ListNode() : ExpressionNode(NodeType::List) {}
  void addExpression(const std::shared_ptr<ASTNode> &expression) {
    _expressions.push_back(expression);
  }
  [[nodiscard]] const std::vector<std::shared_ptr<ASTNode>> &
  getExpressions() const {
    return _expressions;
  }

private:
  std::vector<std::shared_ptr<ASTNode>> _expressions;
};

class QuotedNode final : public ExpressionNode {
public:
  explicit QuotedNode(const std::shared_ptr<ASTNode> &expression)
      : ExpressionNode(NodeType::Quoted), _expression(expression) {}
  [[nodiscard]] const std::shared_ptr<ASTNode> &getExpression() const {
    return _expression;
  }

private:
  std::shared_ptr<ASTNode> _expression;
};

class KeywordNode final : public ExpressionNode {
public:
  explicit KeywordNode(std::string value)
      : ExpressionNode(NodeType::Keyword), _value(std::move(value)) {}
  [[nodiscard]] const std::string &getValue() const { return _value; }

private:
  std::string _value;
};

class IdentifierNode final : public ExpressionNode {
public:
  explicit IdentifierNode(std::string value)
      : ExpressionNode(NodeType::Identifier), _value(std::move(value)) {}
  [[nodiscard]] const std::string &getValue() const { return _value; }

private:
  std::string _value;
};

void printAST(const std::shared_ptr<ASTNode> &node, int indent = 0);

} // namespace parser::ast

#endif // AST_H