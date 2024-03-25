#include <iostream>
#include <optional>
#include <string>
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"


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
    std::cout << content;
    std::vector<Token> tokens = lexer.tokenize(content);
    for(auto &token : tokens){
        if(token.value.has_value()){
            std::cout << "Type: " << token.type << " Value: " << token.value.value() << std::endl;
        }
        else{
            std::cout << "Type: " << token.type << std::endl;
        }
    }

//    std::cout << "Hello, World!" << std::endl;
    return 0;
}
