#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <string>

namespace generator {

inline std::string code_template =
    "#include <iostream>\n#include <memory>\n#include <stdexcept>\n#include "
    "<string>\n#include <utility>\n#include <vector>\n\n// lisp value "
    "types\n#pragma region ValueTypes\n\nenum class Type {\n    Symbol,\n    "
    "Int,\n    Float,\n    String,\n    List,\n    Quoted,\n    T,\n    "
    "Nil,\n};\n\nstruct Value {\n    virtual ~Value() = default;\n    explicit "
    "Value(Type type) : _type(type) {}\n    [[nodiscard]] virtual std::string "
    "get() const = 0;\n    Type _type;\n};\n\nstruct Symbol final : Value {\n  "
    "  explicit Symbol(std::string value)\n        : Value(Type::Symbol), "
    "_value(std::move(value)) {}\n    std::string _value;\n    [[nodiscard]] "
    "std::string get() const override {\n        std::string result;\n        "
    "for (const auto &c : _value)\n            if (islower(c))\n               "
    " result += toupper(c);\n            else\n                result += c;\n  "
    "      return result;\n    }\n};\n\nstruct Int final : Value {\n    "
    "explicit Int(int value) : Value(Type::Int), _value(value) {}\n    int "
    "_value;\n    [[nodiscard]] std::string get() const override {\n        "
    "return std::to_string(_value);\n    }\n};\n\nstruct Float final : Value "
    "{\n    explicit Float(double value) : Value(Type::Float), _value(value) "
    "{}\n    double _value;\n    [[nodiscard]] std::string get() const "
    "override {\n        return std::to_string(_value);\n    }\n};\n\nstruct "
    "String final : Value {\n    explicit String(std::string value)\n        : "
    "Value(Type::String), _value(std::move(value)) {}\n    std::string "
    "_value;\n    [[nodiscard]] std::string get() const override { return '\"' "
    "+ _value + '\"'; }\n};\n\nstruct List final : Value {\n    explicit "
    "List(std::vector<std::shared_ptr<Value>> value)\n        : "
    "Value(Type::List), _value(std::move(value)) {}\n    "
    "std::vector<std::shared_ptr<Value>> _value;\n    [[nodiscard]] "
    "std::string get() const override {\n        std::string result = \"(\";\n "
    "       for (const auto &v : _value)\n            result += v->get() + \" "
    "\";\n        if (!result.empty())\n            result.pop_back();\n       "
    " return result + \")\";\n    }\n};\n\nstruct Quoted final : Value {\n    "
    "explicit Quoted(std::shared_ptr<Value> value)\n        : "
    "Value(Type::Quoted), _value(std::move(value)) {}\n    "
    "std::shared_ptr<Value> _value;\n    [[nodiscard]] std::string get() const "
    "override { return \"'\" + _value->get(); }\n};\n\nstruct T final : Value "
    "{\n    explicit T() : Value(Type::T) {}\n    [[nodiscard]] std::string "
    "get() const override { return \"T\"; }\n};\n\nstruct Nil final : Value "
    "{\n    explicit Nil() : Value(Type::Nil) {}\n    [[nodiscard]] "
    "std::string get() const override { return \"NIL\"; }\n};\n\n#pragma "
    "endregion ValueTypes\n\n// lisp function definition\n#pragma region "
    "FunctionDefinition\n\nstruct Expression {\n    virtual ~Expression() = "
    "default;\n    explicit "
    "Expression(std::vector<std::shared_ptr<Expression>> values)\n        : "
    "_values(std::move(values)) {}\n    [[nodiscard]] virtual "
    "std::shared_ptr<Value> operator()() const = 0;\n\n    "
    "std::vector<std::shared_ptr<Expression>> _values;\n};\n\n#pragma "
    "endregion FunctionDefinition\n\n// lisp type alias\n#pragma region "
    "TypeAlias\n\nusing Variable = std::shared_ptr<Value>;\nusing Function = "
    "std::shared_ptr<Expression>;\nusing Args = "
    "std::vector<std::shared_ptr<Expression>>;\n\n#pragma endregion "
    "TypeAlias\n\n// lisp helper functions\n#pragma region "
    "HelperFunctions\n\n// to_symbol : convert a Value to a "
    "Symbol\n[[nodiscard]] std::shared_ptr<Symbol> to_symbol(const Variable "
    "&v) {\n    if (v->_type == Type::Symbol)\n        return "
    "std::static_pointer_cast<Symbol>(v);\n    throw "
    "std::runtime_error(\"Invalid type conversion\");\n}\n\n// to_int : "
    "convert a Value to an Int\n[[nodiscard]] std::shared_ptr<Int> "
    "to_int(const Variable &v) {\n    if (v->_type == Type::Int)\n        "
    "return std::static_pointer_cast<Int>(v);\n    if (v->_type == "
    "Type::Float)\n        return std::make_shared<Int>(\n            "
    "static_cast<int>(std::static_pointer_cast<Float>(v)->_value));\n    throw "
    "std::runtime_error(\"Invalid type conversion\");\n}\n\n// to_float : "
    "convert a Value to a Float\n[[nodiscard]] std::shared_ptr<Float> "
    "to_float(const Variable &v) {\n    if (v->_type == Type::Float)\n        "
    "return std::static_pointer_cast<Float>(v);\n    if (v->_type == "
    "Type::Int)\n        return std::make_shared<Float>(\n            "
    "static_cast<double>(std::static_pointer_cast<Int>(v)->_value));\n    "
    "throw std::runtime_error(\"Invalid type conversion\");\n}\n\n// to_string "
    ": convert a Value to a String\n[[nodiscard]] std::shared_ptr<String> "
    "to_string(const Variable &v) {\n    if (v->_type == Type::String)\n       "
    " return std::static_pointer_cast<String>(v);\n    throw "
    "std::runtime_error(\"Invalid type conversion\");\n}\n\n// to_list : "
    "convert a Value to a List\n[[nodiscard]] std::shared_ptr<List> "
    "to_list(const Variable &v) {\n    if (v->_type == Type::List)\n        "
    "return std::static_pointer_cast<List>(v);\n    if (v->_type == "
    "Type::Nil)\n        return "
    "std::make_shared<List>(std::vector<std::shared_ptr<Value>>());\n    throw "
    "std::runtime_error(\"Invalid type conversion\");\n}\n\n// to_quoted : "
    "convert a Value to a Quoted\n[[nodiscard]] std::shared_ptr<Quoted> "
    "to_quoted(const Variable &v) {\n    if (v->_type == Type::Quoted)\n       "
    " return std::static_pointer_cast<Quoted>(v);\n    throw "
    "std::runtime_error(\"Invalid type conversion\");\n}\n\n// to_t : convert "
    "a Value to a T\n[[nodiscard]] std::shared_ptr<T> to_t(const Variable &v) "
    "{\n    if (v->_type == Type::T)\n        return "
    "std::static_pointer_cast<T>(v);\n    throw std::runtime_error(\"Invalid "
    "type conversion\");\n}\n\n// to_nil : convert a Value to a "
    "Nil\n[[nodiscard]] std::shared_ptr<Nil> to_nil(const Variable &v) {\n    "
    "if (v->_type == Type::Nil)\n        return "
    "std::static_pointer_cast<Nil>(v);\n    throw std::runtime_error(\"Invalid "
    "type conversion\");\n}\n\n// to_t_or_nil : convert a boolean to a T or "
    "Nil\n[[nodiscard]] std::shared_ptr<Value> to_t_or_nil(bool value) {\n    "
    "if (value)\n        return std::make_shared<T>();\n    return "
    "std::make_shared<Nil>();\n}\n\n#pragma endregion HelperFunctions\n\n// "
    "lisp value functions\n#pragma region ValueFunctions\n\n// symbol : create "
    "a symbol\nstruct SymbolFunction final : public Expression {\n    explicit "
    "SymbolFunction(Args values) = delete;\n    ~SymbolFunction() override = "
    "default;\n    explicit SymbolFunction(std::string atom)\n        : "
    "Expression(std::vector<std::shared_ptr<Expression>>()),\n          "
    "_atom(std::move(atom)) {}\n    Variable operator()() const override {\n   "
    "     if (!_values.empty())\n            throw "
    "std::runtime_error(\"Invalid number of arguments\");\n        return "
    "std::make_shared<Symbol>(_atom);\n    }\n    std::string _atom;\n};\n\n// "
    "int : create an integer\nstruct IntFunction final : public Expression {\n "
    "   explicit IntFunction(Args values) = delete;\n    ~IntFunction() "
    "override = default;\n    explicit IntFunction(const int atom)\n        : "
    "Expression(std::vector<std::shared_ptr<Expression>>()), _atom(atom) {}\n  "
    "  Variable operator()() const override {\n        if (!_values.empty())\n "
    "           throw std::runtime_error(\"Invalid number of arguments\");\n   "
    "     return std::make_shared<Int>(_atom);\n    }\n    int "
    "_atom;\n};\n\n// float : create a float\nstruct FloatFunction final : "
    "public Expression {\n    explicit FloatFunction(Args values) = delete;\n  "
    "  ~FloatFunction() override = default;\n    explicit FloatFunction(const "
    "double atom)\n        : "
    "Expression(std::vector<std::shared_ptr<Expression>>()), _atom(atom) {}\n  "
    "  Variable operator()() const override {\n        if (!_values.empty())\n "
    "           throw std::runtime_error(\"Invalid number of arguments\");\n   "
    "     return std::make_shared<Float>(_atom);\n    }\n    double "
    "_atom;\n};\n\n// string : create a string\nstruct StringFunction final : "
    "public Expression {\n    explicit StringFunction(Args values) = delete;\n "
    "   ~StringFunction() override = default;\n    explicit "
    "StringFunction(std::string atom)\n        : "
    "Expression(std::vector<std::shared_ptr<Expression>>()),\n          "
    "_atom(std::move(atom)) {}\n    Variable operator()() const override {\n   "
    "     if (!_values.empty())\n            throw "
    "std::runtime_error(\"Invalid number of arguments\");\n        return "
    "std::make_shared<String>(_atom);\n    }\n    std::string _atom;\n};\n\n// "
    "list : create a list\nstruct ListFunction final : public Expression {\n   "
    " explicit ListFunction(Args values) : Expression(std::move(values)) {}\n  "
    "  ~ListFunction() override = default;\n    Variable operator()() const "
    "override {\n        std::vector<std::shared_ptr<Value>> result;\n        "
    "for (const auto &v : _values)\n            "
    "result.push_back(v->operator()());\n        return "
    "std::make_shared<List>(result);\n    }\n};\n\n// quoted : create a quoted "
    "value\nstruct QuotedFunction final : public Expression {\n    explicit "
    "QuotedFunction(Args values) : Expression(std::move(values)) {}\n    "
    "~QuotedFunction() override = default;\n    Variable operator()() const "
    "override {\n        if (_values.size() != 1)\n            throw "
    "std::runtime_error(\"Invalid number of arguments\");\n        return "
    "std::make_shared<Quoted>(_values[0]->operator()());\n    }\n};\n\n// t : "
    "create a t value\nstruct TFunction final : public Expression {\n    "
    "explicit TFunction(Args values) = delete;\n    ~TFunction() override = "
    "default;\n    explicit TFunction()\n        : "
    "Expression(std::vector<std::shared_ptr<Expression>>()) {}\n    Variable "
    "operator()() const override {\n        if (!_values.empty())\n            "
    "throw std::runtime_error(\"Invalid number of arguments\");\n        "
    "return std::make_shared<T>();\n    }\n};\n\n// nil : create a nil "
    "value\nstruct NilFunction final : public Expression {\n    explicit "
    "NilFunction(Args values) = delete;\n    ~NilFunction() override = "
    "default;\n    explicit NilFunction()\n        : "
    "Expression(std::vector<std::shared_ptr<Expression>>()) {}\n    Variable "
    "operator()() const override {\n        if (!_values.empty())\n            "
    "throw std::runtime_error(\"Invalid number of arguments\");\n        "
    "return std::make_shared<Nil>();\n    }\n};\n\n#pragma endregion "
    "ValueFunctions\n\n// lisp numeric operations\n#pragma region "
    "NumericOperations\n\n// + : add two numbers\nstruct Add final : public "
    "Expression {\n    explicit Add(Args values) : "
    "Expression(std::move(values)) {}\n    ~Add() override = default;\n    "
    "Variable operator()() const override {\n        if (_values.size() != "
    "2)\n            throw std::runtime_error(\"Invalid number of "
    "arguments\");\n        auto a = _values[0]->operator()();\n        auto b "
    "= _values[1]->operator()();\n        if (a->_type == Type::Int && "
    "b->_type == Type::Int)\n            return "
    "std::make_shared<Int>(to_int(a)->_value + to_int(b)->_value);\n        if "
    "(a->_type == Type::Float && b->_type == Type::Float)\n            return "
    "std::make_shared<Float>(to_float(a)->_value + to_float(b)->_value);\n     "
    "   if (a->_type == Type::Int && b->_type == Type::Float)\n            "
    "return std::make_shared<Float>(to_int(a)->_value + "
    "to_float(b)->_value);\n        if (a->_type == Type::Float && b->_type == "
    "Type::Int)\n            return "
    "std::make_shared<Float>(to_float(a)->_value + to_int(b)->_value);\n       "
    " throw std::runtime_error(\"Invalid type for addition\");\n    "
    "}\n};\n\n// - : subtract two numbers\nstruct Subtract final : public "
    "Expression {\n    explicit Subtract(Args values) : "
    "Expression(std::move(values)) {}\n    ~Subtract() override = default;\n   "
    " Variable operator()() const override {\n        if (_values.size() != "
    "2)\n            throw std::runtime_error(\"Invalid number of "
    "arguments\");\n        auto a = _values[0]->operator()();\n        auto b "
    "= _values[1]->operator()();\n        if (a->_type == Type::Int && "
    "b->_type == Type::Int)\n            return "
    "std::make_shared<Int>(to_int(a)->_value - to_int(b)->_value);\n        if "
    "(a->_type == Type::Float && b->_type == Type::Float)\n            return "
    "std::make_shared<Float>(to_float(a)->_value - to_float(b)->_value);\n     "
    "   if (a->_type == Type::Int && b->_type == Type::Float)\n            "
    "return std::make_shared<Float>(to_int(a)->_value - "
    "to_float(b)->_value);\n        if (a->_type == Type::Float && b->_type == "
    "Type::Int)\n            return "
    "std::make_shared<Float>(to_float(a)->_value - to_int(b)->_value);\n       "
    " throw std::runtime_error(\"Invalid type for subtraction\");\n    "
    "}\n};\n\n// * : multiply two numbers\nstruct Multiply final : public "
    "Expression {\n    explicit Multiply(Args values) : "
    "Expression(std::move(values)) {}\n    ~Multiply() override = default;\n   "
    " Variable operator()() const override {\n        if (_values.size() != "
    "2)\n            throw std::runtime_error(\"Invalid number of "
    "arguments\");\n        auto a = _values[0]->operator()();\n        auto b "
    "= _values[1]->operator()();\n        if (a->_type == Type::Int && "
    "b->_type == Type::Int)\n            return "
    "std::make_shared<Int>(to_int(a)->_value * to_int(b)->_value);\n        if "
    "(a->_type == Type::Float && b->_type == Type::Float)\n            return "
    "std::make_shared<Float>(to_float(a)->_value * to_float(b)->_value);\n     "
    "   if (a->_type == Type::Int && b->_type == Type::Float)\n            "
    "return std::make_shared<Float>(to_int(a)->_value * "
    "to_float(b)->_value);\n        if (a->_type == Type::Float && b->_type == "
    "Type::Int)\n            return "
    "std::make_shared<Float>(to_float(a)->_value * to_int(b)->_value);\n       "
    " throw std::runtime_error(\"Invalid type for multiplication\");\n    "
    "}\n};\n\n// / : divide two numbers\nstruct Divide final : public "
    "Expression {\n    explicit Divide(Args values) : "
    "Expression(std::move(values)) {}\n    ~Divide() override = default;\n    "
    "Variable operator()() const override {\n        if (_values.size() != "
    "2)\n            throw std::runtime_error(\"Invalid number of "
    "arguments\");\n        auto a = _values[0]->operator()();\n        auto b "
    "= _values[1]->operator()();\n        if (a->_type == Type::Int && "
    "b->_type == Type::Int) {\n            if (to_int(b)->_value == 0)\n       "
    "         throw std::runtime_error(\"Division by zero\");\n            if "
    "(to_int(a)->_value % to_int(b)->_value == 0)\n                return "
    "std::make_shared<Int>(to_int(a)->_value / to_int(b)->_value);\n           "
    " return std::make_shared<Float>(static_cast<double>(to_int(a)->_value) "
    "/\n                                           to_int(b)->_value);\n       "
    " }\n        if (a->_type == Type::Float && b->_type == Type::Float)\n     "
    "       return std::make_shared<Float>(to_float(a)->_value / "
    "to_float(b)->_value);\n        if (a->_type == Type::Int && b->_type == "
    "Type::Float)\n            return "
    "std::make_shared<Float>(static_cast<double>(to_int(a)->_value) /\n        "
    "                                   to_float(b)->_value);\n        if "
    "(a->_type == Type::Float && b->_type == Type::Int)\n            return "
    "std::make_shared<Float>(to_float(a)->_value / to_int(b)->_value);\n       "
    " throw std::runtime_error(\"Invalid type for division\");\n    "
    "}\n};\n\n// < : less than\nstruct Less final : public Expression {\n    "
    "explicit Less(Args values) : Expression(std::move(values)) {}\n    "
    "~Less() override = default;\n    Variable operator()() const override {\n "
    "       if (_values.size() != 2)\n            throw "
    "std::runtime_error(\"Invalid number of arguments\");\n        auto a = "
    "_values[0]->operator()();\n        auto b = _values[1]->operator()();\n   "
    "     if (a->_type == Type::Int && b->_type == Type::Int)\n            "
    "return to_t_or_nil(to_int(a)->_value < to_int(b)->_value);\n        if "
    "(a->_type == Type::Float && b->_type == Type::Float)\n            return "
    "to_t_or_nil(to_float(a)->_value < to_float(b)->_value);\n        if "
    "(a->_type == Type::Int && b->_type == Type::Float)\n            return "
    "to_t_or_nil(to_int(a)->_value < to_float(b)->_value);\n        if "
    "(a->_type == Type::Float && b->_type == Type::Int)\n            return "
    "to_t_or_nil(to_float(a)->_value < to_int(b)->_value);\n        throw "
    "std::runtime_error(\"Invalid type for less than\");\n    }\n};\n\n// > : "
    "greater than\nstruct Greater final : public Expression {\n    explicit "
    "Greater(Args values) : Expression(std::move(values)) {}\n    ~Greater() "
    "override = default;\n    Variable operator()() const override {\n        "
    "if (_values.size() != 2)\n            throw std::runtime_error(\"Invalid "
    "number of arguments\");\n        auto a = _values[0]->operator()();\n     "
    "   auto b = _values[1]->operator()();\n        if (a->_type == Type::Int "
    "&& b->_type == Type::Int)\n            return "
    "to_t_or_nil(to_int(a)->_value > to_int(b)->_value);\n        if (a->_type "
    "== Type::Float && b->_type == Type::Float)\n            return "
    "to_t_or_nil(to_float(a)->_value > to_float(b)->_value);\n        if "
    "(a->_type == Type::Int && b->_type == Type::Float)\n            return "
    "to_t_or_nil(to_int(a)->_value > to_float(b)->_value);\n        if "
    "(a->_type == Type::Float && b->_type == Type::Int)\n            return "
    "to_t_or_nil(to_float(a)->_value > to_int(b)->_value);\n        throw "
    "std::runtime_error(\"Invalid type for greater than\");\n    }\n};\n\n// "
    ">= : greater than or equal\nstruct GreaterEqual final : public Expression "
    "{\n    explicit GreaterEqual(Args values) : Expression(std::move(values)) "
    "{}\n    ~GreaterEqual() override = default;\n    Variable operator()() "
    "const override {\n        if (_values.size() != 2)\n            throw "
    "std::runtime_error(\"Invalid number of arguments\");\n        auto a = "
    "_values[0]->operator()();\n        auto b = _values[1]->operator()();\n   "
    "     if (a->_type == Type::Int && b->_type == Type::Int)\n            "
    "return to_t_or_nil(to_int(a)->_value >= to_int(b)->_value);\n        if "
    "(a->_type == Type::Float && b->_type == Type::Float)\n            return "
    "to_t_or_nil(to_float(a)->_value >= to_float(b)->_value);\n        if "
    "(a->_type == Type::Int && b->_type == Type::Float)\n            return "
    "to_t_or_nil(to_int(a)->_value >= to_float(b)->_value);\n        if "
    "(a->_type == Type::Float && b->_type == Type::Int)\n            return "
    "to_t_or_nil(to_float(a)->_value >= to_int(b)->_value);\n        throw "
    "std::runtime_error(\"Invalid type for greater than or equal\");\n    "
    "}\n};\n\n// <= : less than or equal\nstruct LessEqual final : public "
    "Expression {\n    explicit LessEqual(Args values) : "
    "Expression(std::move(values)) {}\n    ~LessEqual() override = default;\n  "
    "  Variable operator()() const override {\n        if (_values.size() != "
    "2)\n            throw std::runtime_error(\"Invalid number of "
    "arguments\");\n        auto a = _values[0]->operator()();\n        auto b "
    "= _values[1]->operator()();\n        if (a->_type == Type::Int && "
    "b->_type == Type::Int)\n            return to_t_or_nil(to_int(a)->_value "
    "<= to_int(b)->_value);\n        if (a->_type == Type::Float && b->_type "
    "== Type::Float)\n            return to_t_or_nil(to_float(a)->_value <= "
    "to_float(b)->_value);\n        if (a->_type == Type::Int && b->_type == "
    "Type::Float)\n            return to_t_or_nil(to_int(a)->_value <= "
    "to_float(b)->_value);\n        if (a->_type == Type::Float && b->_type == "
    "Type::Int)\n            return to_t_or_nil(to_float(a)->_value <= "
    "to_int(b)->_value);\n        throw std::runtime_error(\"Invalid type for "
    "less than or equal\");\n    }\n};\n\n// = : equal\nstruct Equal final : "
    "public Expression {\n    explicit Equal(Args values) : "
    "Expression(std::move(values)) {}\n    ~Equal() override = default;\n    "
    "Variable operator()() const override {\n        if (_values.size() != "
    "2)\n            throw std::runtime_error(\"Invalid number of "
    "arguments\");\n        auto a = _values[0]->operator()();\n        auto b "
    "= _values[1]->operator()();\n        if (a->_type == Type::Int && "
    "b->_type == Type::Int)\n            return to_t_or_nil(to_int(a)->_value "
    "== to_int(b)->_value);\n        if (a->_type == Type::Float && b->_type "
    "== Type::Float)\n            return to_t_or_nil(to_float(a)->_value == "
    "to_float(b)->_value);\n        if (a->_type == Type::Int && b->_type == "
    "Type::Float)\n            return to_t_or_nil(to_int(a)->_value == "
    "to_float(b)->_value);\n        if (a->_type == Type::Float && b->_type == "
    "Type::Int)\n            return to_t_or_nil(to_float(a)->_value == "
    "to_int(b)->_value);\n        throw std::runtime_error(\"Invalid type for "
    "equal\");\n    }\n};\n\n// /= : not equal\nstruct NotEqual final : public "
    "Expression {\n    explicit NotEqual(Args values) : "
    "Expression(std::move(values)) {}\n    ~NotEqual() override = default;\n   "
    " Variable operator()() const override {\n        if (_values.size() != "
    "2)\n            throw std::runtime_error(\"Invalid number of "
    "arguments\");\n        auto a = _values[0]->operator()();\n        auto b "
    "= _values[1]->operator()();\n        if (a->_type == Type::Int && "
    "b->_type == Type::Int)\n            return to_t_or_nil(to_int(a)->_value "
    "!= to_int(b)->_value);\n        if (a->_type == Type::Float && b->_type "
    "== Type::Float)\n            return to_t_or_nil(to_float(a)->_value != "
    "to_float(b)->_value);\n        if (a->_type == Type::Int && b->_type == "
    "Type::Float)\n            return to_t_or_nil(to_int(a)->_value != "
    "to_float(b)->_value);\n        if (a->_type == Type::Float && b->_type == "
    "Type::Int)\n            return to_t_or_nil(to_float(a)->_value != "
    "to_int(b)->_value);\n        throw std::runtime_error(\"Invalid type for "
    "not equal\");\n    }\n};\n\n#pragma endregion NumericOperations\n\n// "
    "lisp logical operations\n#pragma region LogicalOperations\n\n// null : "
    "check if a value is nil\nstruct Null final : public Expression {\n    "
    "explicit Null(Args values) : Expression(std::move(values)) {}\n    "
    "~Null() override = default;\n    Variable operator()() const override {\n "
    "       if (_values.size() != 1)\n            throw "
    "std::runtime_error(\"Invalid number of arguments\");\n        return "
    "to_t_or_nil(_values[0]->operator()()->_type == Type::Nil);\n    "
    "}\n};\n\n// not : check if a value is not nil\nstruct Not final : public "
    "Expression {\n    explicit Not(Args values) : "
    "Expression(std::move(values)) {}\n    ~Not() override = default;\n    "
    "Variable operator()() const override {\n        if (_values.size() != "
    "1)\n            throw std::runtime_error(\"Invalid number of "
    "arguments\");\n        return to_t_or_nil(_values[0]->operator()()->_type "
    "== Type::Nil);\n    }\n};\n\n// if : conditional expression\nstruct If "
    "final : public Expression {\n    explicit If(Args values) : "
    "Expression(std::move(values)) {}\n    ~If() override = default;\n    "
    "Variable operator()() const override {\n        if (_values.size() != "
    "3)\n            throw std::runtime_error(\"Invalid number of "
    "arguments\");\n        auto condition = _values[0]->operator()();\n       "
    " if (condition->_type == Type::Nil ||\n            (condition->_type == "
    "Type::Int && to_int(condition)->_value == 0) ||\n            "
    "(condition->_type == Type::Float && to_float(condition)->_value == 0))\n  "
    "          return _values[2]->operator()();\n        return "
    "_values[1]->operator()();\n    }\n};\n\n#pragma endregion "
    "LogicalOperations\n\n// lisp list operations\n#pragma region "
    "ListOperations\n\n// car : get the first element of a list\nstruct Car "
    "final : public Expression {\n    explicit Car(Args values) : "
    "Expression(std::move(values)) {}\n    ~Car() override = default;\n    "
    "Variable operator()() const override {\n        if (_values.size() != "
    "1)\n            throw std::runtime_error(\"Invalid number of "
    "arguments\");\n        return "
    "to_list(_values[0]->operator()())->_value[0];\n    }\n};\n\n// cdr : get "
    "the rest of the elements of a list\nstruct Cdr final : public Expression "
    "{\n    explicit Cdr(Args values) : Expression(std::move(values)) {}\n    "
    "~Cdr() override = default;\n    Variable operator()() const override {\n  "
    "      if (_values.size() != 1)\n            throw "
    "std::runtime_error(\"Invalid number of arguments\");\n        auto list = "
    "to_list(_values[0]->operator()());\n        if (list->_value.size() < "
    "2)\n            return std::make_shared<Nil>();\n        return "
    "std::make_shared<List>(std::vector<std::shared_ptr<Value>>(\n            "
    "list->_value.begin() + 1, list->_value.end()));\n    }\n};\n\n// cons : "
    "add an element to the front of a list\nstruct Cons final : public "
    "Expression {\n    explicit Cons(Args values) : "
    "Expression(std::move(values)) {}\n    ~Cons() override = default;\n    "
    "Variable operator()() const override {\n        if (_values.size() != "
    "2)\n            throw std::runtime_error(\"Invalid number of "
    "arguments\");\n        auto value = _values[0]->operator()();\n        "
    "auto list = to_list(_values[1]->operator()());\n        "
    "list->_value.insert(list->_value.begin(), value);\n        return list;\n "
    "   }\n};\n\n// list : create a list\nstruct List_ final : public "
    "Expression {\n    explicit List_(Args values) : "
    "Expression(std::move(values)) {}\n    ~List_() override = default;\n    "
    "Variable operator()() const override {\n        "
    "std::vector<std::shared_ptr<Value>> result;\n        for (const auto &v : "
    "_values)\n            result.push_back(v->operator()());\n        return "
    "std::make_shared<List>(result);\n    }\n};\n\n#pragma endregion "
    "ListOperations\n\n// lisp runtime environment\n#pragma region "
    "RuntimeEnvironment\n\n// print : print a value\nstruct Print final : "
    "public Expression {\n    explicit Print(Args values) : "
    "Expression(std::move(values)) {}\n    ~Print() override = default;\n    "
    "Variable operator()() const override {\n        if (_values.size() != "
    "1)\n            throw std::runtime_error(\"Invalid number of "
    "arguments\");\n        auto value = _values[0]->operator()();\n        "
    "std::cout << value->get() << std::endl;\n        return value;\n    "
    "}\n};\n\n// progn : evaluate multiple expressions\nstruct Progn final : "
    "public Expression {\n    explicit Progn(Args values) : "
    "Expression(std::move(values)) {}\n    ~Progn() override = default;\n    "
    "Variable operator()() const override {\n        Variable result = "
    "std::make_shared<Nil>();\n        for (const auto &v : _values)\n         "
    "   result = v->operator()();\n        return result;\n    "
    "}\n};\n\n#pragma endregion RuntimeEnvironment\n\n// definitions\n#pragma "
    "region Definitions\n\n// clang-format off\n\n#define SYMBOL(value) "
    "std::make_shared<SymbolFunction>(value)\n#define INT(value) "
    "std::make_shared<IntFunction>(value)\n#define FLOAT(value) "
    "std::make_shared<FloatFunction>(value)\n#define STRING(value) "
    "std::make_shared<StringFunction>(value)\n#define LIST(...) "
    "std::make_shared<ListFunction>(Args({__VA_ARGS__}))\n#define "
    "QUOTED(value) std::make_shared<QuotedFunction>(Args({value}))\n#define "
    "T() std::make_shared<TFunction>()\n#define NIL() "
    "std::make_shared<NilFunction>()\n#define FUNC(name, ...) "
    "std::make_shared<name>(Args({__VA_ARGS__}))\n#define ARG(number) "
    "_values[number]\n#define DEF(name, args_count, ...)\\\nstruct name final "
    ": public Expression {\\\n    explicit name(Args values) : "
    "Expression(std::move(values)) {\\\n        if (_values.size() != "
    "args_count)\\\n            throw std::runtime_error(\"Invalid number of "
    "arguments\");\\\n    }\\\n    ~name() override = default;\\\n    Variable "
    "operator()() const override {\\\n        return "
    "__VA_ARGS__->operator()();\\\n    }\\\n};\n// clang-format on\n\n#pragma "
    "endregion Definitions\n\n\n$1\n\n\nint main() {\n    try {\n        const "
    "auto program = std::make_shared<Progn>(Args({\n\n            // start of "
    "the program\n\n            $2\n\n            // end of the program\n\n    "
    "    }));\n\n        auto discard = program->operator()();\n\n    } catch "
    "(const std::exception &e) {\n        std::cerr << \"Runtime Error: \" << "
    "e.what() << std::endl;\n        return 1;\n    }\n\n    return 0;\n}";

}; // namespace generator

#endif // TEMPLATE_H