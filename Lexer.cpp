//
// Created by bestd on 3/24/2024.
//

#include <iostream>
#include "Lexer.h"


std::vector<Token> Lexer::tokenize(std::string code) {
    this->code = code;
    for(int i = 0; i < code.length(); i++){
        if(std::isalpha(code[i])){
            std::string word = "";
            while(std::isalnum(code[i]) || std::isdigit(code[i])){
                word += code[i];
                i++;
            }
            if(word == "rawr"){
                tokens.push_back({TokenType::RETURN, std::nullopt});
            }
            else if(word == "funkywunky"){
                tokens.push_back({TokenType::FUNCTION, std::nullopt});
            }
            else if(word == "nyamber"){
                tokens.push_back({TokenType::NUM_VAR, std::nullopt});
            }
            else if(word == "pouncesOnU"){
                tokens.push_back({TokenType::LEFT_PAREN, std::nullopt});
            }
            else if(word == "pouncesOff"){
                tokens.push_back({TokenType::RIGHT_PAREN, std::nullopt});
            }
            else if(word == "noticesBulgyWulgy"){
                tokens.push_back({TokenType::LEFT_BRACE, std::nullopt});
            }
            else if(word == "looksAway"){
                tokens.push_back({TokenType::RIGHT_BRACE, std::nullopt});
            }
            else if(word == "stwing"){
                tokens.push_back({TokenType::STR_VAR, std::nullopt});
            }
            else if(word == "uwu"){
                tokens.push_back({TokenType::ENDLINE, std::nullopt});
            }
            else{
                tokens.push_back({TokenType::IDENTIFIER, word});
            }
        }
        else if(std::isspace(code[i])){
            continue;
        }
        else if(code[i] == '\"'){
            std::string string = "";
            i++;
            while(code[i] != '\"'){
                string += code[i];
                i++;
            }
            tokens.push_back({TokenType::STRING_LITERAL, string});
        }
        else if(std::isdigit(code[i])){
            std::string number = "";
            while(std::isdigit(code[i]) || code[i] == '.'){
                number += code[i];
                i++;
            }
            tokens.push_back({TokenType::NUMBER_LITERAL, number});
        }
        else if(code[i] == '=' || code[i] == '+' || code[i] == '-' || code[i] == '*' || code[i] == '/'){
            std::string binOp = "";
            binOp += code[i];
            tokens.push_back({TokenType::BIN_OP, binOp});
        }
        else if(code[i] == '!'){
            std::string unOp = "";
            unOp += code[i];
            tokens.push_back({TokenType::UN_OP, unOp});
        }
        else{
            std::cerr << "u done goofed" << std::endl;
            exit(1);
        }
    }
    return tokens;
}

void Lexer::printTokens() {

}
