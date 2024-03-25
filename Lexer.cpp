//
// Created by bestd on 3/24/2024.
//

#include <iostream>
#include <utility>
#include <memory>
#include "Lexer.h"

std::vector<Token> Lexer::tokenize(std::string code) {
//    pos = -1;
    this->code   = std::move(code);
    while(peek().has_value()){
        char c = eat();
        if(std::isspace(c)){
            continue;
        }
        if(std::isalpha(c)){
            std::string word;
            word += c;
            while(peek().has_value() && std::isalnum(peek().value())){
                c = eat();
                word += c;
            }
            if(word == "funkywunky") tokens.push_back({TokenType::FUNCTION});
            else if(word == "nyamber") tokens.push_back({TokenType::NUM_VAR});
            else if(word == "stwing") tokens.push_back({TokenType::STR_VAR});
            else if(word == "pouncesOnU") tokens.push_back({TokenType::LEFT_PAREN});
            else if(word == "pouncesOff") tokens.push_back({TokenType::RIGHT_PAREN});
            else if(word == "noticesBulgyWulgy") tokens.push_back({TokenType::LEFT_BRACE});
            else if(word == "looksAway") tokens.push_back({TokenType::RIGHT_BRACE});
            else if(word == "rawr") tokens.push_back({TokenType::RETURN});
            else if(word == "uwu") tokens.push_back({TokenType::ENDLINE});
            else tokens.push_back({TokenType::IDENTIFIER, word});
        }

        else if(c == '=' || c == '/' || c == '*' || c == '+' || c == '-' || c == '^' ){
            std::string Op(1, c);

            if(peek().has_value() && peek().value() == '='){
                c = eat();
                Op += c;
            }
            if(c == '+' || c == '-' && peek().has_value() && peek().value() == c){
                c = eat();
                Op += c;
                tokens.push_back({TokenType::UN_OP, Op});
            }
            else{
                tokens.push_back({TokenType::BIN_OP, Op});
            }
        }
        else if(std::isdigit(c)){
            std::string number;
            number += c;
            while(peek().has_value() && (std::isdigit(peek().value()) || peek().value() == '.')){
                c = eat();
                number += c;
            }
            tokens.push_back({TokenType::NUMBER_LITERAL, number});
        }
        else if(c == '\"'){
            std::string string;
            while(peek().has_value() && peek().value() != '\"'){
                c = eat();
                string += c;
            }
            eat(); // eat the \"
            tokens.push_back({TokenType::STRING_LITERAL, string});
        }
    }
    return tokens;
}

void Lexer::printTokens() {

}

std::optional<char> Lexer::peek(int ahead) {
    if(pos + ahead > code.length() - 1){return std::nullopt;}
    return code[pos + ahead];;
}

char Lexer::eat() {
    return code.at(pos++);
}
