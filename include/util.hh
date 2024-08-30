
#pragma once

#include <filesystem>

#include "token.hh"

typedef struct {
    std::string name;
    std::filesystem::path location;
    char *code;
} CodeFile;

std::string tokenKindToString(TokenKind kind);
void prettyPrintToken(Token token);

bool isValidNumber(std::string base, const char* c);

std::string operatorToString(TokenKind op);
