#ifndef TOKEN_H
#define TOKEN_H
#include <optional>
#include <string>

namespace lexer::token {

enum class TokenType {
  Greater, // symbols
  GreaterEqual,
  Less,
  LessEqual,
  Equal,
  NotEqual,
  Plus,
  Minus,
  Times,
  Divide,
  LParen,
  RParen,
  Quote,
  Null, // keywords
  Not,
  If,
  Let,
  DefineFunction,
  Car,
  Cdr,
  T,
  Nil,
  Cons,
  List,
  Progn,
  Print,
  Identifier, // atoms
  Integer,
  Floating,
  String,
  END // end of tokens
};

class Token {
public:
  explicit Token(const TokenType type,
                 const std::optional<std::string> &value = std::nullopt)
      : _type(type), _value(value) {}

  [[nodiscard]] TokenType getType() const { return _type; }
  [[nodiscard]] std::optional<std::string> getValue() const { return _value; }

private:
  TokenType _type;
  std::optional<std::string> _value;
};

} // namespace lexer::token

#endif // TOKEN_H