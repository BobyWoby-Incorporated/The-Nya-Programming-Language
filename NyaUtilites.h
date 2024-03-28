//
// Created by bestd on 3/24/2024.
//

#ifndef NYA_NYAUTILITES_H
#define NYA_NYAUTILITES_H

#include <optional>
#include <string>

enum TokenType{
    RETURN,
    ENDLINE,
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    NUMBER_LITERAL,
    STRING_LITERAL,
    IDENTIFIER,
    FUNCTION,
    NUM_VAR,
    STR_VAR,
    BIN_OP,
    UN_OP,
    CALL,
    COMMA,
    NULL_VAL,
    EXTERN,


};
struct Token{
    TokenType type;
    std::optional<std::string> value;
};
#endif //NYA_NYAUTILITES_H
