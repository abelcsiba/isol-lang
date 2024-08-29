
#pragma once

#include <string_view>
#include <string>
#include <filesystem>
#include <vector>

#include "token.hh"
#include "util.hh"


class Lexer {
private:
    std::string_view code;
    size_t lex_begin;
    size_t lex_curr;
    Location loc;
    CodeFile file;

    TokenList token_list;

    // ----- Helpers -----
    bool isEof();
    bool isWhitespace();
    bool isDigit();
    bool isShadower();
    bool isAlpha();
    bool isAlphaNumeric();
    
    // ----- Tokenizers -----
    void advance(int offset = 1);
    bool adjustPos(int offset = 1);
    const char peek(int offset = 1);
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
    Lexer(const char* raw_code, CodeFile file);

    TokenList getTokens();

    bool lex();    
};
