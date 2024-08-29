
#pragma once

#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <map>

#include "token.hh"
#include "ast.hh"


class Parser {
private:
    TokenList token_list;
    size_t parse_curr;
    ASTModule* module;

    using PrefixParseFn = std::function<ExprPtr(Parser&)>;
    using InfixParseFn = std::function<ExprPtr(Parser&, ExprPtr)>;

    std::unordered_map<TokenKind, PrefixParseFn> prefixParseFns;
    std::unordered_map<TokenKind, InfixParseFn> infixParseFns;
    std::unordered_map<TokenKind, int> precedences;

    void registerPrefix(TokenKind kind, PrefixParseFn fn);
    void registerInfix(TokenKind kind, InfixParseFn fn, uint8_t precedence);

    ExprPtr parseExpression(uint8_t precedence = 0);
    ExprPtr parseNumber();
    ExprPtr parseIdentifier();
    ExprPtr parseGroup();
    ExprPtr parseBinaryOp(ExprPtr left);
    ExprPtr parseAssignment();
    ExprPtr parseFunctionCall(ExprPtr left);
    ExprPtr parseIndexing(ExprPtr left);

    // ----- Helpers -----
    bool isEof();
    void advance(size_t offset = 1);
    Token peek(size_t offset = 1);
    Token previous();

    // ----- Parsers -----
    bool parseModule();
    bool parseImport();

public:
    Parser(TokenList tokens);
    bool parse();

    ASTModule* getModule();
};