//
// Created by BobyWoby on 3/24/2024.
//

#include "Parser.h"
#include <memory>

std::unique_ptr<NumberExprAST> Parser::parseNumberLiteral(Token token){
    double val = std::stod(token.value.value());
    return std::make_unique<NumberExprAST>(val);
}

std::unique_ptr<StringExprAST> Parser::parseStringLiteral(Token token){
    return std::make_unique<StringExprAST>(token.value.value());
}
std::unique_ptr<ExprAST> Parser::parseExpression(std::vector<Token> *tokens, int index){

}

void Parser::parse(std::vector<Token> tokens){
    for(int i = 0; i < tokens.size(); i++) {
        Token token = tokens.at(i);
        if (token.type == TokenType::FUNCTION) {
            //parseFunction();
        } else if (token.type == TokenType::NUM_VAR) {
            //parseNumVar();
        } else if (token.type == TokenType::STR_VAR) {
            //parseStrVar();
        } else if (token.type == TokenType::RETURN) {
            //parseReturn();
        } else if (token.type == TokenType::ENDLINE) {
            //parseEndline();
        } else if (token.type == TokenType::IDENTIFIER) {
            //parseIdentifier();
        } else if (token.type == TokenType::BIN_OP) {
            //parseBinOp();
        } else if (token.type == TokenType::UN_OP) {
            //parseUnOp();
        } else if (token.type == TokenType::NUMBER_LITERAL) {
            parseNumberLiteral(token);
        } else if (token.type == TokenType::STRING_LITERAL) {
            parseStringLiteral(token);
        }
    }
}