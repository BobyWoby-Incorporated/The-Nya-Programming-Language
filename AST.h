//
// Created by bestd on 3/24/2024.
//

#ifndef NYA_AST_H
#define NYA_AST_H

#include <memory>

enum class ExprType{
    Number,
    String,
    Variable,
    Binary,
    Call,
    Prototype,
    Function
};

class ExprAST {
public:
    virtual ~ExprAST() = default;
    Token token;
};

class NumberExprAST : ExprAST{
    double val;
public:
    NumberExprAST(double val) : val(val) {token = {TokenType::NUMBER_LITERAL};}
};

class StringExprAST : ExprAST{
    std::string val;
public:
    StringExprAST(std::string val) : val(val) {token = {TokenType::STRING_LITERAL};}
};

class VariableExprAST : ExprAST{
    std::string name;
    TokenType dataType;
public:
    VariableExprAST(std::string name, TokenType dataType) : name(std::move(name)), dataType(dataType) {token = {dataType};}
};

class BinaryExprAST : ExprAST{
    char op;
    std::unique_ptr<ExprAST> lhs, rhs;
public:
    BinaryExprAST(char op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs) :
    op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {token = {TokenType::BIN_OP};}
};

class TypeExprAST : ExprAST{
    TokenType dataType;
    std::string name;
public:
    TypeExprAST(TokenType dataType, std::string name) :
    dataType(dataType), name(std::move(name)) {token = {dataType, this->name};}
};

class CallExprAST : ExprAST{
    std::string callee;
    std::vector<std::pair<std::unique_ptr<TypeExprAST>, std::unique_ptr<ExprAST>>> args;

public:
    CallExprAST(std::string callee,std::vector<std::pair<std::unique_ptr<TypeExprAST>, std::unique_ptr<ExprAST>>> args)
    : callee(std::move(callee)), args(std::move(args)) {token = {TokenType::CALL, this->callee};}
};

class PrototypeExprAST : ExprAST{
    std::string name;
    std::vector<std::pair<std::unique_ptr<TypeExprAST>, std::unique_ptr<ExprAST>>> args;
public:
    PrototypeExprAST(std::string name,  std::vector<std::pair<std::unique_ptr<TypeExprAST>, std::unique_ptr<ExprAST>>> args) :
    name(std::move(name)), args(std::move(args)) {token = {TokenType::PROTOTYPE, this->name};}
};

class FunctionExprAST : ExprAST{
    std::unique_ptr<PrototypeExprAST> proto;
    std::unique_ptr<ExprAST> body;
public:
    FunctionExprAST(std::unique_ptr<PrototypeExprAST> proto, std::unique_ptr<ExprAST> body) : proto(std::move(proto)), body(std::move(body)) {type = ExprType::Function;}
};

#endif //NYA_AST_H