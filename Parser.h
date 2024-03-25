//
// Created by BobyWoby on 3/24/2024.
//

#ifndef NYA_PARSER_H
#define NYA_PARSER_H
#include <string>
#include "NyaUtilites.h"
#include <vector>
#include "AST.h"


class Parser {
public:
    Parser() = default;
    ~Parser() = default;
    void parse(std::vector<Token> tokens);
private:

    static std::unique_ptr<NumberExprAST> parseNumberLiteral(Token token);

    static std::unique_ptr<StringExprAST> parseStringLiteral(Token token);

    std::unique_ptr<ExprAST> parseExpression(std::vector<Token> *tokens, int index);

    std::unique_ptr<ExprAST> ParsePrimary(Token token);
};


#endif //NYA_PARSER_H
