
#pragma once

#include <vector>
#include <map>

#include "token.hh"
#include "ast.hh"

class Parser {
private:
    std::vector<Token> token_list;
    size_t parse_curr;
    ASTModule *module;

    // ----- Helpers -----
    bool isEof();
    void advance(size_t offset = 1);
    Token peek(size_t offset = 1);

    // ----- Parsers -----
    bool parseModule();
    ASTExpression parseExpression();

public:
    Parser(std::vector<Token> tokens);
    bool parse();
    ASTModule* getModule();
};