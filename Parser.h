//
// Created by BobyWoby on 3/24/2024.
//

#ifndef NYA_PARSER_H
#define NYA_PARSER_H
#include <string>
#include "NyaUtilites.h"
#include <vector>
#include <map>
#include "AST.h"


class Parser {
public:
    Parser();
    ~Parser() = default;
    std::vector<std::unique_ptr<ExprAST>> parse(std::vector<Token> tokens);
private:
    size_t index = 0;

    std::vector<Token> tokens;

    std::map<std::string, int> binopPrecedence;

    Token eat();

    std::optional<Token> tryEat(TokenType type);

    std::optional<Token> peek(int ahead = 1);

    std::unique_ptr<NumberExprAST> parseNumberLiteral();

    std::unique_ptr<StringExprAST> parseStringLiteral();

    std::unique_ptr<PrototypeExprAST> parsePrototype();

    std::unique_ptr<ExprAST> parseExpression();

    std::unique_ptr<ExprAST> parsePrimary();

    std::unique_ptr<ExprAST> parseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> lhs);

    std::unique_ptr<ExprAST> parseParenExpr();

    void logError(std::string msg);

    std::unique_ptr<ExprAST> parseIdentifier();

    int getTokPrecedence();

    std::unique_ptr<FunctionExprAST> parseDefintion();

    std::unique_ptr<PrototypeExprAST> parseExtern();

    std::unique_ptr<FunctionExprAST> parseTopLevelExpr();

    std::unique_ptr<ExprAST> parseReturnExpr();

    std::unique_ptr<ScopeExprAST> parseScope();
};


#endif //NYA_PARSER_H
