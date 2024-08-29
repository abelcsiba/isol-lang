
#pragma once

#include <string>
#include <vector>


typedef enum /* TokenErrorCode */ {
    #include "err_codes.def"
} TokenErrorCode;

typedef enum /* TokenKind */ {
    #include "tokens.def"
} TokenKind;

typedef struct /* Location */ {
    int col;
    int row;
} Location;

typedef struct /* Token */ {
    TokenKind kind;
    char *lexeme;
    Location location;
    TokenErrorCode err;
    bool shadower = false;
} Token;

using TokenList = std::vector<Token>;

void newToken(Token &token, const int col, const int row, TokenKind kind, TokenErrorCode err);
