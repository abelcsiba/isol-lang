
#pragma once

#include <string>


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
} Token;

void newToken(Token &token, const int col, const int row, TokenKind kind, TokenErrorCode err);
