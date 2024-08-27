#ifndef __LEXER_HH__
#define __LEXER_HH__


#include <string_view>
#include <string>
#include <filesystem>
#include <vector>

#include "token.hh"


typedef struct {
    std::string name;
    std::filesystem::path location;
} CodeFile;

class Lexer {
private:
    std::string_view code;
    int lex_begin;
    int lex_curr;
    Location loc;
    CodeFile file;

    std::vector<Token> token_list;

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
    Token nextToken();
    void addEofToken();

public: 
    Lexer(const char* raw_code, CodeFile file);

    std::vector<Token> getTokens();

    bool lex();    
};

#endif