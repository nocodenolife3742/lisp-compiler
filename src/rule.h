#ifndef RULE_H
#define RULE_H
#include <tao/pegtl.hpp>
#include <unordered_map>

namespace lexer::rule {

namespace pegtl = tao::pegtl;

// clang-format off

// helper
struct Sign : pegtl::one<'+', '-'> {};
struct Space : pegtl::plus<pegtl::space> {};

// symbols
struct Greater : pegtl::one<'>'> {};
struct GreaterEqual : pegtl::string<'>', '='> {};
struct Less : pegtl::one<'<'> {};
struct LessEqual : pegtl::string<'<', '='> {};
struct Equal : pegtl::one<'='> {};
struct NotEqual : pegtl::string<'/', '='> {};
struct Plus : pegtl::one<'+'> {};
struct Minus : pegtl::one<'-'> {};
struct Times : pegtl::one<'*'> {};
struct Divide : pegtl::one<'/'> {};
struct LParen : pegtl::one<'('> {};
struct RParen : pegtl::one<')'> {};
struct Quote : pegtl::one<'\''> {};
struct Symbol : pegtl::sor<GreaterEqual, LessEqual, NotEqual, Greater, Less, Equal, Plus, Minus, Times, Divide, LParen, RParen, Quote> {};

// atoms
struct Integer : pegtl::seq<pegtl::opt<pegtl::one<'+', '-'>>, pegtl::plus<pegtl::digit>> {};
struct Floating : pegtl::seq<pegtl::opt<pegtl::one<'+', '-'>>, pegtl::plus<pegtl::digit>, pegtl::one<'.'>, pegtl::plus<pegtl::digit>> {};
struct String : pegtl::seq<pegtl::one<'"'>, pegtl::star<pegtl::sor<pegtl::seq<pegtl::one<'\\'>, pegtl::any>, pegtl::not_one<'"', '\\'>>>, pegtl::one<'"'>> {};
struct Identifier : pegtl::plus<pegtl::sor<pegtl::alnum>> {};

// rules
struct Unknown : pegtl::any {};
struct Token : pegtl::sor<Space, Symbol, String, Floating, Integer, Identifier, Unknown> {};
struct Grammar : pegtl::seq<pegtl::star<Token>, pegtl::eof> {};

// clang-format on

// actions

std::unordered_map<std::string, token::TokenType> keywords = {
    {"null", token::TokenType::Null},
    {"not", token::TokenType::Not},
    {"if", token::TokenType::If},
    {"let", token::TokenType::Let},
    {"defun", token::TokenType::DefineFunction},
    {"car", token::TokenType::Car},
    {"cdr", token::TokenType::Cdr},
    {"t", token::TokenType::T},
    {"nil", token::TokenType::Nil},
    {"cons", token::TokenType::Cons},
    {"list", token::TokenType::List},
    {"progn", token::TokenType::Progn},
    {"print", token::TokenType::Print}};

template <typename Rule> struct Action {};

template <> struct Action<Identifier> {
  template <typename Input>

  static void apply(const Input &in, std::vector<token::Token> &out) {
    if (keywords.contains(in.string()))
      out.emplace_back(keywords.at(in.string()), in.string());
    else
      out.emplace_back(token::TokenType::Identifier, in.string());
  }
};

template <> struct Action<GreaterEqual> {
  template <typename Input>
  static void apply(const Input &in, std::vector<token::Token> &out) {
    out.emplace_back(token::TokenType::GreaterEqual);
  }
};

template <> struct Action<LessEqual> {
  template <typename Input>
  static void apply(const Input &in, std::vector<token::Token> &out) {
    out.emplace_back(token::TokenType::LessEqual);
  }
};

template <> struct Action<NotEqual> {
  template <typename Input>
  static void apply(const Input &in, std::vector<token::Token> &out) {
    out.emplace_back(token::TokenType::NotEqual);
  }
};

template <> struct Action<Greater> {
  template <typename Input>
  static void apply(const Input &in, std::vector<token::Token> &out) {
    out.emplace_back(token::TokenType::Greater);
  }
};

template <> struct Action<Less> {
  template <typename Input>
  static void apply(const Input &in, std::vector<token::Token> &out) {
    out.emplace_back(token::TokenType::Less);
  }
};

template <> struct Action<Equal> {
  template <typename Input>
  static void apply(const Input &in, std::vector<token::Token> &out) {
    out.emplace_back(token::TokenType::Equal);
  }
};

template <> struct Action<Plus> {
  template <typename Input>
  static void apply(const Input &in, std::vector<token::Token> &out) {
    out.emplace_back(token::TokenType::Plus);
  }
};

template <> struct Action<Minus> {
  template <typename Input>
  static void apply(const Input &in, std::vector<token::Token> &out) {
    out.emplace_back(token::TokenType::Minus);
  }
};

template <> struct Action<Times> {
  template <typename Input>
  static void apply(const Input &in, std::vector<token::Token> &out) {
    out.emplace_back(token::TokenType::Times);
  }
};

template <> struct Action<Divide> {
  template <typename Input>
  static void apply(const Input &in, std::vector<token::Token> &out) {
    out.emplace_back(token::TokenType::Divide);
  }
};

template <> struct Action<LParen> {
  template <typename Input>
  static void apply(const Input &in, std::vector<token::Token> &out) {
    out.emplace_back(token::TokenType::LParen);
  }
};

template <> struct Action<RParen> {
  template <typename Input>
  static void apply(const Input &in, std::vector<token::Token> &out) {
    out.emplace_back(token::TokenType::RParen);
  }
};

template <> struct Action<Quote> {
  template <typename Input>
  static void apply(const Input &in, std::vector<token::Token> &out) {
    out.emplace_back(token::TokenType::Quote);
  }
};

template <> struct Action<Integer> {
  template <typename Input>
  static void apply(const Input &in, std::vector<token::Token> &out) {
    out.emplace_back(token::TokenType::Integer, in.string());
  }
};

template <> struct Action<Floating> {
  template <typename Input>
  static void apply(const Input &in, std::vector<token::Token> &out) {
    out.emplace_back(token::TokenType::Floating, in.string());
  }
};

template <> struct Action<String> {
  template <typename Input>
  static void apply(const Input &in, std::vector<token::Token> &out) {
    std::string value;
    bool escape = false;
    for (const auto &c : in.string().substr(1, in.size() - 2)) {
      if (c == '\\' && !escape)
        escape = true;
      else
        value += c, escape = false;
    }
    out.emplace_back(token::TokenType::String, value);
  }
};

template <> struct Action<Unknown> {
  template <typename Input>
  static void apply(const Input &in, std::vector<token::Token> &out) {
    throw std::runtime_error("Could not lex: " + in.string());
  }
};

} // namespace lexer::rule

namespace helper::rule {

namespace pegtl = tao::pegtl;

// clang-format off

// comments
struct SingleLineComment : pegtl::seq<pegtl::string<';'>, pegtl::until<pegtl::eolf>> {};
struct MultiLineComment : pegtl::seq<pegtl::string<'#', '|'>, pegtl::until<pegtl::string<'|', '#'>>> {};
struct Comment : pegtl::sor<SingleLineComment, MultiLineComment> {};

// () -> nil
struct Nil : pegtl::string<'(', ')'> {};

// rules
struct Unknown : pegtl::any {};
struct Grammar : pegtl::seq<pegtl::star<pegtl::sor<Comment, Nil, Unknown>>, pegtl::eof> {};

// clang-format on

// actions
template <typename Rule> struct Action {};

template <> struct Action<Unknown> {
  template <typename Input>
  static void apply(const Input &in, std::string &out) {
    out += in.string();
  }
};

template <> struct Action<Nil> {
  template <typename Input>
  static void apply(const Input &in, std::string &out) {
    out += "nil";
  }
};

} // namespace helper::rule

#endif // RULE_H