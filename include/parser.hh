
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

    using PrefixParseFn = std::function<ExprPtr(Parser&, Token&)>;
    using InfixParseFn = std::function<ExprPtr(Parser&, ExprPtr)>;

    std::unordered_map<TokenKind, PrefixParseFn> prefixParseFns;
    std::unordered_map<TokenKind, InfixParseFn> infixParseFns;
    std::unordered_map<TokenKind, int> precedences;

    void registerPrefix(TokenKind kind, PrefixParseFn fn);
    void registerInfix(TokenKind kind, InfixParseFn fn, uint8_t precedence);
    uint8_t getPrecedence(TokenKind kind);

    ExprPtr parseExpression(uint8_t precedence = 0);
    ExprPtr parseNumber(Token &token);
    ExprPtr parseIdentifier(Token &token);
    ExprPtr parseGroup(Token &token);
    ExprPtr parseBinaryOp(ExprPtr left);
    ExprPtr parseUnary(Token &token);
    ExprPtr parseAssignment();
    ExprPtr parseFunctionCall(ExprPtr left);
    ExprPtr parseIndexing(ExprPtr left);

    StmtPtr parseVarDeclaration();
    StmtPtr parseIfStatement();
    StmtPtr parseBlockStatement();


    // ----- Helpers -----
    bool isEof();
    Token advance();
    void consume(size_t offset = 1);
    Token peek(size_t offset = 1);
    Token previous();
    bool match(TokenKind kind);

    // ----- Parsers -----
    bool parseModule();
    bool parseImport();

public:
    Parser(TokenList tokens);
    bool parse();

    ASTModule* getModule();
};