
#pragma once

#include <string>
#include <vector>

typedef enum /* TypeKind */ {
    PT_I8           = 0,
    PT_I16          = 1,
    PT_I32          = 2,
    PT_I64          = 3,
    PT_FLOAT        = 4,
    PT_CHAR         = 5,
    PT_STRING       = 6,
    PT_BOOL         = 7,
    CT_Composite    = 8,
} TypeKind;

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
    char * lexeme; // TODO: replace it with std::string! It is leaking memory.
    Location location;
    TokenErrorCode err;
    bool shadower = false;

    bool is(TokenKind k) { return kind == k; }
    bool isOneOf(TokenKind k1, TokenKind k2) { return is(k1) || is(k2); }

    template <typename... Ts>
    bool isOneOf(TokenKind k1, TokenKind k2, Ts... ks) { return is(k1) || isOneOf(k2, ks...); }
} Token;

typedef struct /* TypeInfo */ {
    TypeKind kind;
    std::string type_name;
} TypeInfo;

using TokenList = std::vector<Token>;

void newToken(Token &token, const int col, const int row, TokenKind kind, TokenErrorCode err);
