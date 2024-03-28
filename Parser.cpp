//
// Created by BobyWoby on 3/24/2024.
//

#include "Parser.h"
#include <memory>
#include <iostream>

std::unique_ptr<NumberExprAST> Parser::parseNumberLiteral(){
    return std::make_unique<NumberExprAST>(eat());
}

std::unique_ptr<StringExprAST> Parser::parseStringLiteral(){
    return std::make_unique<StringExprAST>(eat());
}

std::unique_ptr<ExprAST> Parser::parsePrimary(){
    switch(tokens.at(index).type){
        case IDENTIFIER:
            return parseIdentifier();
        case NUMBER_LITERAL:
            return parseNumberLiteral();
        case STRING_LITERAL:
            return parseStringLiteral();
        case LEFT_PAREN:
            return parseParenExpr();
        default:
            logError("Oopsie woopsie! Nyaa~ (´•ω•`) ♡ U didn't give me a token, silly!");
    }
}

int Parser::getTokPrecedence() {
    int tokPrec = binopPrecedence[peek(0).value().value.value()];
    if(tokPrec <= 0) return -1;
    return tokPrec;
}

std::unique_ptr<ExprAST> Parser::parseParenExpr(){
    auto token = tryEat(TokenType::LEFT_PAREN); // eat ( or {;
    if(!token.has_value()){
//        token = tryEat(TokenType::LEFT_BRACE);
        logError("U-uh! Nyaa, u need a '(', dummy! (つ✧ω✧)つ♡");
    }
    auto V = parseExpression();
    if(!V)
        return nullptr;
    if(tokens.at(index).type != token.value().type){
        logError((token.value().type == TokenType::RIGHT_PAREN)? "U-uh! Nyaa, u need a ')', dummy! (つ✧ω✧)つ♡" : "U-uh! Nyaa, u need a '}', dummy! (つ✧ω✧)つ♡");
    }
    return V;

}
std::unique_ptr<ExprAST> Parser::parseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> lhs){
    while(true) {
        int tokPrec = getTokPrecedence();
        if(tokPrec < ExprPrec) return lhs;
        std::string binOp = peek(0).value().value.value();
        auto rhs = parsePrimary();
        if(!rhs) return nullptr;
        int nextPrec = getTokPrecedence();
        if(tokPrec < nextPrec){
            rhs = parseBinOpRHS(tokPrec + 1, std::move(rhs));
            if(!rhs)
                return nullptr;
        }
        lhs = std::make_unique<BinaryExprAST>(binOp, std::move(lhs), std::move(rhs));
    }
}

std::unique_ptr<ExprAST> Parser::parseExpression(){
    auto lhs = parsePrimary();
    if(!lhs) return nullptr;
    return parseBinOpRHS(0, std::move(lhs));
}

std::unique_ptr<ExprAST> Parser::parseIdentifier(){
    Token tmp = eat(); // eat the identifier
    if(tokens.at(index).type != TokenType::LEFT_PAREN && (tokens.at(index - 1).type == TokenType::NUM_VAR || tokens.at(index - 1).type == TokenType::STR_VAR)){
        return std::make_unique<VariableExprAST>(tmp.value.value(), tokens.at(index - 1).type);
    }
    eat(); // eat (
    std::vector<std::unique_ptr<ExprAST>> Args;
    if(peek(0).value().type != TokenType::RIGHT_PAREN){
        while(true) {
            if(auto Arg = parseExpression()) Args.push_back(std::move(Arg));
            else{
                return nullptr;
            }
            if(peek(0).has_value() && peek(0).value().type == TokenType::RIGHT_PAREN){
                break;
            }
            if(peek(0).has_value() && peek(0).value().type != TokenType::COMMA){
                logError("Expewcted ')' or ',' in awgument wist~ nya! o(≧∇≦o) ♡"); // Expected ')' or ',' in argument list
            }
            eat();
        }
    }
    eat(); // eat the )
    return std::make_unique<CallExprAST>(tmp.value.value(), std::move(Args));
}


std::unique_ptr<PrototypeExprAST> Parser::parsePrototype() {
    // funkywunky [nyamber] main pouncesOnU nyamber test pouncesOff

    if(peek(0).value().type != TokenType::STR_VAR && peek(0).value().type != TokenType::NUM_VAR) logError("Expected return type in prototype");
    TokenType returnType = peek(0).value().type;
    eat();

    if(peek(0).value().type != TokenType::IDENTIFIER) logError("Ewww, wat a nerd, he didn't even name his funkywunky 눈_눈");
    std::string FnName = peek(0).value().value.value();
    eat();

    if(peek(0).value().type != TokenType::LEFT_PAREN) logError("Awww, u not gonna pounce on after making a funkywunky? (っ-‸-ς )");

    std::vector<std::unique_ptr<VariableExprAST>> Args;
    while(peek().value().type == TokenType::STR_VAR || peek().value().type == TokenType::NUM_VAR){
        // funkywunky nyamber main pouncesOnU [nyamber] test pouncesOff
        TokenType argType = eat().type; // eat argument type
        if(!peek().has_value() || peek().value().type != IDENTIFIER) logError("Uh Oh, seems like someone forgor to name his pawameters ฅ(^•ﻌ•^)ฅ");
        // funkywunky nyamber main pouncesOnU nyamber [test] pouncesOff
        std::string argName = tryEat(TokenType::IDENTIFIER).value().value.value();
        // funkywunky nyamber main pouncesOnU nyamber test [pouncesOff]
        Args.push_back(std::make_unique<VariableExprAST>(argName, argType));
    }
    if(peek(0).value().type != TokenType::RIGHT_PAREN) logError("My guy, u gotta pounce off, or else things might get legally questionable (⚆д⚆)");
    return std::make_unique<PrototypeExprAST>(FnName, returnType, std::move(Args));
}

std::unique_ptr<FunctionExprAST> Parser::parseDefintion() {
    // [funkywunky] nyamber main pouncesOnU nyamber test pouncesOff
    eat(); // eat def
    auto Proto = parsePrototype();
    if(!Proto) return nullptr;

    if(auto E = parseExpression())
        return std::make_unique<FunctionExprAST>(std::move(Proto), std::move(E));
    return nullptr;
}
std::unique_ptr<PrototypeExprAST> Parser::parseExtern() {
    eat(); // eat éxtern
    return parsePrototype();
}

std::unique_ptr<FunctionExprAST> Parser::parseTopLevelExpr() {
    if(auto E = parseExpression()) {
        auto Proto = std::make_unique<PrototypeExprAST>("", TokenType::NULL_VAL, std::vector<std::unique_ptr<VariableExprAST>>());
        return std::make_unique<FunctionExprAST>(std::move(Proto), std::move(E));
    }
    return nullptr;
}

void Parser::parse(std::vector<Token> inToks){
    tokens = std::move(inToks);
    while(true){
        if(!peek().has_value()) return;
        switch(peek(0).value().type){
            case TokenType::FUNCTION:
                parseDefintion();
                break;
            case TokenType::ENDLINE:
                eat();
                break;
            case TokenType::EXTERN:
                parseExtern();
            default:
                parseTopLevelExpr();
                break;
        }
    }
}

std::optional<Token> Parser::peek(int ahead) {
    if(index + ahead > tokens.size() - 1){return std::nullopt;}
    return tokens.at(index + ahead);
}

Token Parser::eat() {
    return tokens.at(index++);
}

std::optional<Token> Parser::tryEat(TokenType type) {
    if (peek().has_value() && peek().value().type == type) {
        return eat();
    }
    return {};
}

void Parser::logError(std::string msg) {
    std::cerr << msg << std::endl;
    exit(1);
}

Parser::Parser() {
    binopPrecedence["+="] = 5;
    binopPrecedence["-="] = 5;
    binopPrecedence["*="] = 5;
    binopPrecedence["/="] = 5;

    binopPrecedence["=="] = 10;
    binopPrecedence["<"] = 10;
    binopPrecedence[">"] = 10;

    binopPrecedence["+"] = 20;
    binopPrecedence["-"] = 20;
    binopPrecedence["*"] = 40;
    binopPrecedence["/"] = 40;

}

