#include <iostream>
#include <optional>
#include <string>
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"


std::string TokenTypeStringFormat[]{
        "RETURN",
        "ENDLINE",
        "LEFT_PAREN",
        "RIGHT_PAREN",
        "LEFT_BRACE",
        "RIGHT_BRACE",
        "NUMBER_LITERAL",
        "STRING_LITERAL",
        "IDENTIFIER",
        "FUNCTION",
        "NUM_VAR",
        "STR_VAR",
        "BIN_OP",
        "UN_OP",
};

std::string toString(Token token){
    std::string str = TokenTypeStringFormat[token.type];
    if(token.value.has_value()){
        str += ", Value: " + token.value.value();
    }
    return str;
}

int main(int argc, char **argv) {
    Lexer lexer{};
    Parser parser{};
    if(argc < 2){
        std::cout << "Usage: nya <filename>";
        exit(1);
    }
    std::fstream file(argv[1], std::ios::in);
    if(!file.is_open()){
        std::cout << "Couldn't open file";
        exit(1);
    }
    std::string content;
    std::stringstream buffer;

    buffer << file.rdbuf();
    content = buffer.str();
    std::vector<Token> tokens = lexer.tokenize(content);

    for(auto &token : tokens){
        std::cout << toString(token) << std::endl;
    }

//    std::cout << "Hello, World!" << std::endl;
    return 0;
}
