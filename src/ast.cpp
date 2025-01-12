#include "ast.h"
#include <iostream>
#include <memory>

void parser::ast::printAST(const std::shared_ptr<ASTNode> &node, int indent) {
  std::cout << std::string(indent, ' ');
  switch (node->getType()) {
  case NodeType::Program: {
    std::cout << "Program" << std::endl;
    for (const auto &expression :
         std::static_pointer_cast<ProgramNode>(node)->getExpressions())
      printAST(expression, indent + 4);
    break;
  }

  case NodeType::Integer: {
    std::cout << "Integer: "
              << std::static_pointer_cast<IntegerNode>(node)->getValue()
              << std::endl;
    break;
  }

  case NodeType::Floating: {
    std::cout << "Floating: "
              << std::static_pointer_cast<FloatingNode>(node)->getValue()
              << std::endl;
    break;
  }

  case NodeType::String: {
    std::cout << "String: "
              << std::static_pointer_cast<StringNode>(node)->getValue()
              << std::endl;
    break;
  }

  case NodeType::List: {
    std::cout << "List" << std::endl;
    const auto &list = std::static_pointer_cast<ListNode>(node);
    for (const auto &expression : list->getExpressions())
      printAST(expression, indent + 4);
    break;
  }
  case NodeType::Quoted: {
    std::cout << "Quoted" << std::endl;
    printAST(std::static_pointer_cast<QuotedNode>(node)->getExpression(),
             indent + 4);
    break;
  }
  case NodeType::Keyword: {
    std::cout << "Keyword: "
              << std::static_pointer_cast<KeywordNode>(node)->getValue()
              << std::endl;
    break;
  }
  case NodeType::Identifier: {
    std::cout << "Identifier: "
              << std::static_pointer_cast<IdentifierNode>(node)->getValue()
              << std::endl;
    break;
  }
  }
}
