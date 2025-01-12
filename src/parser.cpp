#include "parser.h"
#include "rule.h"
#include <tao/pegtl/contrib/analyze.hpp>

void parser::Parser::preprocess() {
  std::string output;
  tao::pegtl::memory_input<> input(_source, "source");
  assert(tao::pegtl::analyze<helper::rule::Grammar>() == 0);
  tao::pegtl::parse<helper::rule::Grammar, helper::rule::Action>(input, output);
  _source = output;
}

void parser::Parser::tokenize() {
  tao::pegtl::memory_input<> input(_source, "source");
  assert(tao::pegtl::analyze<lexer::rule::Grammar>() == 0);
  tao::pegtl::parse<lexer::rule::Grammar, lexer::rule::Action>(input, _tokens);
}

std::shared_ptr<parser::ast::ASTNode> parser::Parser::parse() {
  preprocess();
  tokenize();
  parseTokens();
  return _ast;
}

void parser::Parser::parseTokens() { _ast = parseProgram(); }

void parser::Parser::advance() { _index++; }

lexer::token::Token parser::Parser::currentToken() {
  if (_index < _tokens.size())
    return _tokens[_index];
  return lexer::token::Token(lexer::token::TokenType::END);
}

std::shared_ptr<parser::ast::ASTNode> parser::Parser::parseProgram() {
  auto program = std::make_shared<ast::ProgramNode>();
  while (currentToken().getType() != lexer::token::TokenType::END)
    program->addExpression(parseExpression());
  return program;
}

std::shared_ptr<parser::ast::ASTNode> parser::Parser::parseExpression() {
  switch (currentToken().getType()) {
  case lexer::token::TokenType::LParen:
    return parseList();
  case lexer::token::TokenType::Quote:
    return parseQuoted();
  case lexer::token::TokenType::Identifier:
    return parseIdentifier();
  case lexer::token::TokenType::Integer:
  case lexer::token::TokenType::Floating:
  case lexer::token::TokenType::String:
    return parseLiteral();
  case lexer::token::TokenType::Null:
  case lexer::token::TokenType::Not:
  case lexer::token::TokenType::If:
  case lexer::token::TokenType::Let:
  case lexer::token::TokenType::DefineFunction:
  case lexer::token::TokenType::Car:
  case lexer::token::TokenType::Cdr:
  case lexer::token::TokenType::T:
  case lexer::token::TokenType::Nil:
  case lexer::token::TokenType::Cons:
  case lexer::token::TokenType::List:
  case lexer::token::TokenType::Progn:
  case lexer::token::TokenType::Print:
  case lexer::token::TokenType::Greater:
  case lexer::token::TokenType::GreaterEqual:
  case lexer::token::TokenType::Less:
  case lexer::token::TokenType::LessEqual:
  case lexer::token::TokenType::Equal:
  case lexer::token::TokenType::NotEqual:
  case lexer::token::TokenType::Plus:
  case lexer::token::TokenType::Minus:
  case lexer::token::TokenType::Times:
  case lexer::token::TokenType::Divide:
    return parseKeyword();
  default:
    throw std::runtime_error("Unexpected token at parsing expression");
  }
}

std::shared_ptr<parser::ast::ASTNode> parser::Parser::parseList() {
  advance();
  auto list = std::make_shared<ast::ListNode>();
  while (currentToken().getType() != lexer::token::TokenType::RParen)
    list->addExpression(parseExpression());
  advance();
  return list;
}

std::shared_ptr<parser::ast::ASTNode> parser::Parser::parseLiteral() {
  const auto token = currentToken();
  advance();
  switch (token.getType()) {
  case lexer::token::TokenType::Integer:
    return std::make_shared<ast::IntegerNode>(
        std::stoi(token.getValue().value()));
  case lexer::token::TokenType::Floating:
    return std::make_shared<ast::FloatingNode>(
        std::stod(token.getValue().value()));

  case lexer::token::TokenType::String:
    return std::make_shared<ast::StringNode>(token.getValue().value());
  default:
    throw std::runtime_error("Unexpected token at parsing literal");
  }
}

std::shared_ptr<parser::ast::ASTNode> parser::Parser::parseQuoted() {
  advance();
  return std::make_shared<ast::QuotedNode>(parseExpression());
}

std::shared_ptr<parser::ast::ASTNode> parser::Parser::parseIdentifier() {
  const auto token = currentToken();
  advance();
  assert(token.getValue().has_value());
  return std::make_shared<ast::IdentifierNode>(token.getValue().value());
}

std::shared_ptr<parser::ast::ASTNode> parser::Parser::parseKeyword() {
  const auto token = currentToken();
  advance();
  switch (token.getType()) {
  case lexer::token::TokenType::Null:
    return std::make_shared<ast::KeywordNode>("null");
  case lexer::token::TokenType::Not:
    return std::make_shared<ast::KeywordNode>("not");
  case lexer::token::TokenType::If:
    return std::make_shared<ast::KeywordNode>("if");
  case lexer::token::TokenType::Let:
    return std::make_shared<ast::KeywordNode>("let");
  case lexer::token::TokenType::DefineFunction:
    return std::make_shared<ast::KeywordNode>("defun");
  case lexer::token::TokenType::Car:
    return std::make_shared<ast::KeywordNode>("car");
  case lexer::token::TokenType::Cdr:
    return std::make_shared<ast::KeywordNode>("cdr");
  case lexer::token::TokenType::T:
    return std::make_shared<ast::KeywordNode>("t");
  case lexer::token::TokenType::Nil:
    return std::make_shared<ast::KeywordNode>("nil");
  case lexer::token::TokenType::Cons:
    return std::make_shared<ast::KeywordNode>("cons");
  case lexer::token::TokenType::List:
    return std::make_shared<ast::KeywordNode>("list");
  case lexer::token::TokenType::Progn:
    return std::make_shared<ast::KeywordNode>("progn");
  case lexer::token::TokenType::Print:
    return std::make_shared<ast::KeywordNode>("print");
  case lexer::token::TokenType::Greater:
    return std::make_shared<ast::KeywordNode>(">");
  case lexer::token::TokenType::GreaterEqual:
    return std::make_shared<ast::KeywordNode>(">=");
  case lexer::token::TokenType::Less:
    return std::make_shared<ast::KeywordNode>("<");
  case lexer::token::TokenType::LessEqual:
    return std::make_shared<ast::KeywordNode>("<=");
  case lexer::token::TokenType::Equal:
    return std::make_shared<ast::KeywordNode>("=");
  case lexer::token::TokenType::NotEqual:
    return std::make_shared<ast::KeywordNode>("/=");
  case lexer::token::TokenType::Plus:
    return std::make_shared<ast::KeywordNode>("+");
  case lexer::token::TokenType::Minus:
    return std::make_shared<ast::KeywordNode>("-");
  case lexer::token::TokenType::Times:
    return std::make_shared<ast::KeywordNode>("*");
  case lexer::token::TokenType::Divide:
    return std::make_shared<ast::KeywordNode>("/");
  default:
    throw std::runtime_error("Unexpected token at parsing keyword");
  }
}
