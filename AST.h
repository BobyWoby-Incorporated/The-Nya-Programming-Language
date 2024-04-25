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

static std::unique_ptr<llvm::LLVMContext> TheContext;
static std::unique_ptr<llvm::IRBuilder<>> Builder;
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
        return nullptr;
    }
//    explicit ScopeExprAST(std::vector<std::unique_ptr<ExprAST>> body) : body(std::move(body)) {type = {TokenType::LEFT_BRACE};}
    explicit ScopeExprAST(std::vector<std::unique_ptr<ExprAST>> body) : body(std::move(body)) {type = {TokenType::LEFT_BRACE};}
};

class ReturnExprAST : public ExprAST {
public:
//    Token retTok;
    std::unique_ptr<ExprAST> retExpr;
    llvm::Value *codegen() override{
        return Builder->CreateRet(retExpr->codegen());
    }
    explicit ReturnExprAST(std::unique_ptr<ExprAST> retExpr) : retExpr(std::move(retExpr)) {type = {TokenType::RETURN};}
};

class NumberExprAST : public ExprAST{
    Token token;
public:
    llvm::Value *codegen() override {
        return llvm::ConstantFP::get(*TheContext, llvm::APFloat(stod(token.value.value())));
    };
    explicit NumberExprAST(Token token) : token(token) {type = token.type;}
};

class StringExprAST : public ExprAST{
    Token token;
public:
    llvm::Value *codegen() override {
        return Builder->CreateGlobalString(llvm::StringRef(token.value.value()));
    };
    explicit StringExprAST(Token token) : token(token) {type = token.type;}
};

class VariableExprAST : public ExprAST{
    std::string name;
    TokenType dataType;
public:
    std::string getName(){return name;}
    llvm::Value * codegen() override {
        llvm::Value *V = NamedValues[std::tuple<TokenType, std::string>(dataType, name)];
        if(!V) {
            V = NamedValues[std::tuple<TokenType, std::string>(TokenType::IDENTIFIER, name)];
        }
        if(!V){
            logErrorAST("This vawiable doesn't exist! (つ✧ω✧)つ♡ Nyaa~ ♡");
        }
        return V;
    };
    VariableExprAST(std::string name, TokenType dataType) : name(std::move(name)), dataType(dataType){this->type = dataType;}
};

class BinaryExprAST : public ExprAST{
    std::string op;
    std::unique_ptr<ExprAST> lhs, rhs;
public:
    llvm::Value * codegen() override{
        llvm::Value *L = lhs->codegen();
        llvm::Value *R = rhs->codegen();
        if(!L || !R) return nullptr;
        if(op == "+"){
            if(lhs->type == TokenType::STR_VAR && rhs->type == TokenType::STR_VAR){
                return Builder->CreateGlobalString(L->getName().str() + R->getName().str());
            }
            else if(lhs->type == TokenType::NUM_VAR){
                return Builder->CreateFAdd(L, R, "addtmp");
            }
            else if(lhs->type == TokenType::NUMBER_LITERAL && rhs->type == TokenType::NUMBER_LITERAL){
                return Builder->CreateFAdd(L, R, "addtmp");
            }
            else if(lhs->type == TokenType::STRING_LITERAL && rhs->type == TokenType::STRING_LITERAL){
                return Builder->CreateGlobalString(L->getName().str() + R->getName().str());
            }
            else if(L->getType()->isFloatingPointTy() && R->getType()->isFloatingPointTy()){
                return Builder->CreateFAdd(L, R, "addtmp");
            }
            else if(L->getType()->isPointerTy() && R->getType()->isPointerTy()){
                return Builder->CreateGlobalString(L->getName().str() + R->getName().str());
            }
            else{
                logErrorAST("Invalid operation");
            }
        }
        if(op == "="){
            llvm::Value *V = NamedValues[std::tuple<TokenType, std::string>(lhs->type, lhs->type == TokenType::NUM_VAR ? lhs->codegen()->getName().str() : lhs->codegen()->getName().str())];
            if(!V){
                logErrorAST("This vawiable doesn't exist! (つ✧ω✧)つ♡ Nyaa~ ♡");
            }
            return Builder->CreateStore(R, V);
        }
        if(op == "-") return Builder->CreateFSub(L, R, "subtmp");
        if(op == "*") return Builder->CreateFMul(L, R, "multmp");
        if(op == "/") return Builder->CreateFDiv(L, R, "divtmp");
        if(op == "<") return Builder->CreateFCmpULT(L, R, "cmptmp");
        if(op == ">") return Builder->CreateFCmpULT(R, L, "cmptmp");
        return nullptr;
    }
    BinaryExprAST(std::string op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs): op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)){ this->type = BIN_OP; }
};

class CallExprAST : public ExprAST{
    std::string callee;
    std::vector<std::unique_ptr<ExprAST>> args;

public:
    llvm::Value * codegen() override{
        llvm::Function *CalleeF = TheModule->getFunction(callee);
        if(!CalleeF) logErrorAST("Unknown function referenced");
        if(CalleeF->arg_size() != args.size()) logErrorAST("Incorrect number of arguments passed");
        std::vector<llvm::Value *> ArgsV;
        for(auto &arg : args){
            ArgsV.push_back(arg->codegen());
            if(!ArgsV.back()) return nullptr;
        }
        return Builder->CreateCall(CalleeF, ArgsV, "calltmp");
    }
    CallExprAST(std::string callee, std::vector<std::unique_ptr<ExprAST>> args) : callee(std::move(callee)), args(std::move(args)) {}
};

class PrototypeExprAST : public ExprAST{
    std::string name;
    TokenType returnType;
    std::vector<std::unique_ptr<VariableExprAST>> args;
public:
    TokenType getReturnType() {return returnType;}
    std::string getName() {return name;}
    llvm::Function * codegen() override{
        std::vector<llvm::Type *> types;
        for(auto &arg : args){
            if(arg->type == TokenType::NUM_VAR){
                types.push_back(llvm::Type::getDoubleTy(*TheContext));
            }
            else if(arg->type == TokenType::STR_VAR){
                // strings are an array of chars
                types.push_back(llvm::PointerType::get(llvm::Type::getInt8Ty(*TheContext), 0));
            }
        }
        llvm::FunctionType *FT;
        if(returnType == TokenType::NUM_VAR){
            FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*TheContext), types, false);
        }
        else if(returnType == TokenType::STR_VAR){
            FT = llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*TheContext), 0), types, false);
        }
        llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name, TheModule.get());
        unsigned Idx = 0;
        for(auto &Arg : F->args()){
            Arg.setName(args[Idx++]->getName());
        }
        return F;
    }
    PrototypeExprAST(std::string n, TokenType returnType, std::vector<std::unique_ptr<VariableExprAST>> args): name(n), returnType(returnType), args(std::move(args)){
        std::cout << "name: " << name << std::endl;
    }
};

class FunctionExprAST : public ExprAST{
    std::unique_ptr<PrototypeExprAST> proto;
    std::unique_ptr<ExprAST> body;
public:
    llvm::Function * codegen() override{
        llvm::Function *TheFunction = TheModule->getFunction(proto->getName());
        if(!TheFunction) TheFunction = proto->codegen();
        if(!TheFunction) {
            std::cerr << "Function is null" << std::endl;
            return nullptr;
        }
        if(!TheFunction->empty()) logErrorAST("Function cannot be redefined");
        llvm::BasicBlock *BB = llvm::BasicBlock::Create(*TheContext, "entry", TheFunction);
        Builder->SetInsertPoint(BB);
        NamedValues.clear();
        for(auto &Arg : TheFunction->args()){
            NamedValues[std::tuple<TokenType, std::string>(proto->getReturnType(), Arg.getName().str())] = &Arg;
            NamedValues[std::tuple<TokenType, std::string>(TokenType::IDENTIFIER, Arg.getName().str())] = &Arg;
        }
        //codegen the scope that is the body of the function
        body->codegen();
        llvm::verifyFunction(*TheFunction);
        return TheFunction;
    }
    FunctionExprAST(std::unique_ptr<PrototypeExprAST> proto, std::unique_ptr<ExprAST> body) : proto(std::move(proto)), body(std::move(body)){}

};

#endif //NYA_AST_H