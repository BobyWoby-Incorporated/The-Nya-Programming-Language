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

};


#endif //NYA_PARSER_H
