
#pragma once

#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <map>

#include "token.hh"
#include "ast.hh"
#include "diagnostics.hh"

class Parser {
private:
    CodeFile *file;
    size_t parse_curr;
    ASTModule* module;
    Diagnostics *diag;

    using PrefixParseFn = std::function<ExprPtr(Parser&, Token&)>;
    using InfixParseFn = std::function<ExprPtr(Parser&, ExprPtr, bool)>;

    std::unordered_map<TokenKind, PrefixParseFn> prefixParseFns;
    std::unordered_map<TokenKind, InfixParseFn> infixParseFns;
    std::unordered_map<TokenKind, int> precedences;

    void registerPrefix(TokenKind kind, PrefixParseFn fn);
    void registerInfix(TokenKind kind, InfixParseFn fn, uint8_t precedence);
    uint8_t getPrecedence(TokenKind kind);

    ExprPtr parseExpression(uint8_t precedence = 0, bool allowAssignment = true);
    ExprPtr parseNumber(Token &token);
    ExprPtr parseString(Token &token);
    ExprPtr parseChar(Token &token);
    ExprPtr parseIdentifier(Token &token);
    ExprPtr parseGroup();
    ExprPtr parseBinaryOp(ExprPtr left, bool allowAssignment);
    ExprPtr parseUnary(Token &token);
    ExprPtr parseAssignment(ExprPtr left, bool allowAssignment);
    ExprPtr parseFunctionCall(ExprPtr left, bool allowAssignment);
    ExprPtr parseIndexing(ExprPtr left, bool allowAssignment);
    ExprPtr parseInvocation(ExprPtr left, bool allowAssignment);

    StmtPtr parseVarDeclaration();
    StmtPtr parseIfStatement();
    StmtPtr parseBlockStatement();
    StmtPtr parseStatement();
    StmtPtr parseEntry();

    TypeInfo parseTypeInfo();


    // ----- Helpers -----
    bool isEof();
    Token advance();
    void consume(size_t offset = 1);
    Token peek(size_t offset = 1);
    Token previous();
    bool match(TokenKind kind);
    Message report(std::string message, std::string other_info = "", Token *token = nullptr);

    // ----- Parsers -----
    bool parseModule();
    bool parseImport();

public:
    Parser(CodeFile *file);
    bool parse();
    void setDiag(Diagnostics *diagnostics);

    ASTModule* getModule();
};