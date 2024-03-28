//
// Created by bestd on 3/24/2024.
//

#ifndef NYA_AST_H
#define NYA_AST_H

#include <memory>
#include <utility>
#include <variant>


class ExprAST {
public:
    TokenType type;
    virtual ~ExprAST() = default;
};

class NumberExprAST : public ExprAST{
    Token token;
public:
    explicit NumberExprAST(Token token) : token(token) {type = token.type;}
};

class StringExprAST : public ExprAST{
    Token token;
public:
    explicit StringExprAST(Token token) : token(token) {type = token.type;}
};

class VariableExprAST : public ExprAST{
    std::string name;
    TokenType dataType;
public:
    VariableExprAST(std::string name, TokenType dataType) : name(std::move(name)), dataType(dataType){}
};

class BinaryExprAST : public ExprAST{
    std::string op;
    std::unique_ptr<ExprAST> lhs, rhs;
public:
    BinaryExprAST(std::string op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs): op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)){}
};


class CallExprAST : public ExprAST{
    std::string callee;
    std::vector<std::unique_ptr<ExprAST>> args;

public:
    CallExprAST(std::string callee, std::vector<std::unique_ptr<ExprAST>> args) : callee(std::move(callee)), args(std::move(args)) {}
};

class PrototypeExprAST : public ExprAST{
    std::string name;
    TokenType returnType;
    std::vector<std::unique_ptr<VariableExprAST>> args;
public:
    PrototypeExprAST(std::string name, TokenType returnType, std::vector<std::unique_ptr<VariableExprAST>> args): name(std::move(name)), returnType(returnType), args(std::move(args)){}
};

class FunctionExprAST : public ExprAST{
    std::unique_ptr<PrototypeExprAST> proto;
    std::unique_ptr<ExprAST> body;
public:
    FunctionExprAST(std::unique_ptr<PrototypeExprAST> proto, std::variant<std::unique_ptr<ExprAST>, std::unique_ptr<BinaryExprAST>, std::unique_ptr<VariableExprAST>> body){}

};

#endif //NYA_AST_H