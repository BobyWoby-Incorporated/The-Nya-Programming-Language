//
// Created by bestd on 3/24/2024.
//

#ifndef NYA_LEXER_H
#define NYA_LEXER_H

#include <optional>
#include <string>
#include <vector>
#include "NyaUtilites.h"

class Lexer {
public:
    Lexer() = default;
    ~Lexer() = default;
    std::vector<Token> tokenize(std::string code);
    void printTokens();
private:
    std::string code;
    std::vector<Token> tokens;
};


#endif //NYA_LEXER_H
