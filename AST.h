//
// Created by bestd on 3/24/2024.
//

#ifndef NYA_AST_H
#define NYA_AST_H
#include <iostream>
#include <memory>
#include <utility>
#include <variant>
#include <tuple>
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

static llvm::LLVMContext TheContext;
static llvm::IRBuilder<> Builder(TheContext);
static std::unique_ptr<llvm::Module> TheModule;
static std::map<std::tuple<TokenType, std::string>, llvm::Value *> NamedValues;



class ExprAST {
public:
    void logErrorAST(const std::string& e){
        std::cerr << e << std::endl;
        exit(1);
    }
    TokenType type;
    virtual llvm::Value *codegen() = 0;
    virtual ~ExprAST() = default;
};

class ScopeExprAST : public ExprAST {
    std::vector<std::unique_ptr<ExprAST>> body;
public:
    llvm::Value *codegen() override{
        for(auto &expr : body){
            expr->codegen();
        }
    }
    explicit ScopeExprAST(std::vector<std::unique_ptr<ExprAST>> body) : body(std::move(body)) {type = {TokenType::LEFT_BRACE};}
};

class ReturnExprAST : public ExprAST {
public:
//    Token retTok;
    std::unique_ptr<ExprAST> retExpr;

    llvm::Value *codegen() override{
        return Builder.CreateRetVoid();
    }
    explicit ReturnExprAST(std::unique_ptr<ExprAST> retExpr) : retExpr(std::move(retExpr)) {type = {TokenType::RETURN};}
};

class NumberExprAST : public ExprAST{
    Token token;
public:
    llvm::Value *codegen() override {
        return llvm::ConstantFP::get(TheContext, llvm::APFloat(stod(token.value.value())));
    };
    explicit NumberExprAST(Token token) : token(token) {type = token.type;}
};

class StringExprAST : public ExprAST{
    Token token;
public:
    llvm::Value *codegen() override {
        return Builder.CreateGlobalString(llvm::StringRef(token.value.value()));
    };
    explicit StringExprAST(Token token) : token(token) {type = token.type;}
};

class VariableExprAST : public ExprAST{
    std::string name;
    TokenType dataType;
public:
    llvm::Value * codegen() override {
        llvm::Value *V = NamedValues[std::tuple<TokenType, std::string>(dataType, name)];
        if(!V){
            logErrorAST("This vawiable doesn't exist! (つ✧ω✧)つ♡ Nyaa~ ♡");
        }
        return V;
    };
    VariableExprAST(std::string name, TokenType dataType) : name(std::move(name)), dataType(dataType){}
};

class BinaryExprAST : public ExprAST{
    std::string op;
    std::unique_ptr<ExprAST> lhs, rhs;
public:
    llvm::Value * codegen() override{
        llvm::Value *L = lhs->codegen();
        llvm::Value *R = rhs->codegen();
        if(!L || !R) return nullptr;
        if(op == "+") return Builder.CreateFAdd(L, R, "addtmp");
        if(op == "-") return Builder.CreateFSub(L, R, "subtmp");
        if(op == "*") return Builder.CreateFMul(L, R, "multmp");
        if(op == "/") return Builder.CreateFDiv(L, R, "divtmp");
        if(op == "<") return Builder.CreateFCmpULT(L, R, "cmptmp");
        if(op == ">") return Builder.CreateFCmpULT(R, L, "cmptmp");

    }
    BinaryExprAST(std::string op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs): op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)){}
};


class CallExprAST : public ExprAST{
    std::string callee;
    std::vector<std::unique_ptr<ExprAST>> args;

public:
    llvm::Value * codegen() override{

    }
    CallExprAST(std::string callee, std::vector<std::unique_ptr<ExprAST>> args) : callee(std::move(callee)), args(std::move(args)) {}
};

class PrototypeExprAST : public ExprAST{
    std::string name;
    TokenType returnType;
    std::vector<std::unique_ptr<VariableExprAST>> args;
public:
    llvm::Value * codegen() override{}
    PrototypeExprAST(std::string name, TokenType returnType, std::vector<std::unique_ptr<VariableExprAST>> args): name(std::move(name)), returnType(returnType), args(std::move(args)){}
};

class FunctionExprAST : public ExprAST{
    std::unique_ptr<PrototypeExprAST> proto;
    std::unique_ptr<ExprAST> body;
public:
    llvm::Value * codegen() override{}
    FunctionExprAST(std::unique_ptr<PrototypeExprAST> proto, std::variant<std::unique_ptr<ExprAST>, std::unique_ptr<BinaryExprAST>, std::unique_ptr<VariableExprAST>> body){}

};

#endif //NYA_AST_H