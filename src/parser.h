#ifndef PARSER_H
#define PARSER_H
#include "ast.h"
#include "token.h"
#include <memory>
#include <string>
#include <vector>

namespace parser {

class Parser {
public:
  explicit Parser(std::string source)
      : _source(std::move(source)), _ast(nullptr) {}
  std::shared_ptr<ast::ASTNode> parse();

private:
  void preprocess();
  void tokenize();
  void parseTokens();
  std::string _source;
  std::vector<lexer::token::Token> _tokens;
  std::shared_ptr<ast::ASTNode> _ast;
  size_t _index = 0;

  // Parser functions
  void advance();
  lexer::token::Token currentToken();
  std::shared_ptr<ast::ASTNode> parseProgram();
  std::shared_ptr<ast::ASTNode> parseList();
  std::shared_ptr<ast::ASTNode> parseLiteral();
  std::shared_ptr<ast::ASTNode> parseExpression();
  std::shared_ptr<ast::ASTNode> parseQuoted();
  std::shared_ptr<ast::ASTNode> parseIdentifier();
  std::shared_ptr<ast::ASTNode> parseKeyword();
};

} // namespace parser

#endif // PARSER_H
