
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
int64_t getNumericValue(std::string num);

std::string operatorToString(TokenKind op);
std::string parseEscapeSequences(std::string_view data);

