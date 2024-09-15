
#pragma once

#include <string_view>
#include <string>
#include <filesystem>
#include <vector>

#include "token.hh"
#include "util.hh"
#include "diagnostics.hh"


class Lexer {
private:
    std::string_view code;
    size_t lex_begin;
    size_t lex_curr;
    Location loc;
    CodeFile *file;
    Diagnostics *diag;

    // ----- Helpers -----
    bool isEof();
    bool isWhitespace();
    bool isDigit(size_t offset = 0);
    bool isShadower();
    bool isAlpha();
    bool isAlphaNumeric();
    Message report(std::string message, std::string other_info = "", Token *token = nullptr);
    
    // ----- Tokenizers -----
    void advance(int offset = 1);
    bool adjustPos(int offset = 1);
    char peek(int offset = 1);
    Token consume(TokenKind kind, int offset = 0);
    bool match(const char *keyword);
    void matchKeywords(Token &token);
    void eatMultilineComment(Token &token);
    void eatSinglelineComment(Token &token);
    void eatIdentifier(Token &token);
    void eatNumber(Token &token);
    void eatCharLiteral(Token &token);
    void eatStringLiteral(Token &token);
    Token nextToken();
    void addEofToken();

public: 
    Lexer(CodeFile *file);

    void setDiag(Diagnostics *diagnostics);

    bool lex();    
};
