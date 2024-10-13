
#include <iostream>
#include <cstddef>

#include "parser.hh"


Parser::Parser(CodeFile *file)
{
    this->parse_curr = 0;
    this->file = file;

    registerPrefix(TOKEN_NUM_LITERAL, [](Parser& p, Token &prev) { return p.parseNumber(prev); });
    registerPrefix(TOKEN_FLOAT_LITERAL, [](Parser& p, Token &prev) { return p.parseNumber(prev); });
    registerPrefix(TOKEN_STRING_LITERAL, [](Parser& p, Token &prev) { return p.parseString(prev); });
    registerPrefix(TOKEN_CHAR_LITERAL, [](Parser &p, Token &prev) { return p.parseChar(prev); });
    registerPrefix(TOKEN_IDENTIFIER, [](Parser& p, Token &prev) { return p.parseIdentifier(prev); }); 
    registerPrefix(TOKEN_LEFT_PAREN, [](Parser& p, Token& /*prev*/) { return p.parseGroup(); });
    registerPrefix(TOKEN_MINUS, [](Parser& p, Token &prev) { return p.parseUnary(prev); });
    registerPrefix(TOKEN_PLUS, [](Parser& p, Token &prev) { return p.parseUnary(prev); });
    registerPrefix(TOKEN_STAR, [](Parser &p, Token &prev) { return p.parseUnary(prev); });
    registerPrefix(TOKEN_AND, [](Parser &p, Token &prev) { return p.parseUnary(prev); });

    registerInfix(TOKEN_EQUAL, [](Parser& p, ExprPtr left, bool allowAssignment = true) { return p.parseAssignment(std::move(left), allowAssignment); }, 5);
    registerInfix(TOKEN_PLUS, [](Parser& p, ExprPtr left, bool allowAssignment = true) { return p.parseBinaryOp(std::move(left), allowAssignment); }, 10);
    registerInfix(TOKEN_MINUS, [](Parser& p, ExprPtr left, bool allowAssignment = true) { return p.parseBinaryOp(std::move(left), allowAssignment); }, 10);
    registerInfix(TOKEN_STAR, [](Parser& p, ExprPtr left, bool allowAssignment = true) { return p.parseBinaryOp(std::move(left), allowAssignment); }, 20);
    registerInfix(TOKEN_SLASH, [](Parser& p, ExprPtr left, bool allowAssignment = true) { return p.parseBinaryOp(std::move(left), allowAssignment); }, 20);
    registerInfix(TOKEN_DOT, [](Parser &p, ExprPtr left, bool allowAssignment = true) { return p.parseInvocation(std::move(left), allowAssignment); }, 25);
    registerInfix(TOKEN_LEFT_PAREN, [](Parser& p, ExprPtr left, bool allowAssignment = true) { return p.parseFunctionCall(std::move(left), allowAssignment); }, 30);
    registerInfix(TOKEN_LEFT_BRACKET, [](Parser& p, ExprPtr left, bool allowAssignment = true) { return p.parseIndexing(std::move(left), allowAssignment); }, 30);
}

bool Parser::isEof()
{
    return file->tokens[parse_curr].kind == TOKEN_EOF;
}

Token Parser::advance()
{
    return file->tokens[parse_curr++];
}

void Parser::consume(size_t offset)
{
    parse_curr += offset;
}

Token Parser::peek(size_t offset)
{

    if (parse_curr + offset >= file->tokens.size()) 
    {
        // TODO: This type of error handling is probably insufficient
        Token token;
        token.kind = TOKEN_EOF;
        return token;
    }
    return file->tokens[parse_curr + offset];
}

Token Parser::previous()
{
    return file->tokens[parse_curr - 1]; 
}

bool Parser::match(TokenKind kind)
{
    return advance().kind == kind;
}

bool Parser::parse()
{
    this->module = new ASTModule();
    while (!isEof())
    {      
        Token token = peek(0);

        if (token.kind == TOKEN_PURE) std::cout << "Parsing pure func." << std::endl;     // TODO: Missing implementation
        else if (token.kind == TOKEN_RECORD) std::cout << "Parsing record." << std::endl; // TODO: Missing implementation
        else if (token.kind == TOKEN_ENTITY) std::cout << "Parsing entity." << std::endl; // TODO: Missing implementation
        else if (token.kind == TOKEN_ENTRY) 
        { 
            this->module->entry = parseEntry(); 
            
            if (!this->module->entry) 
            {
                diag->error(report("Syntax error in entry declaration!"));
                return false;
            }
        }
        else if (token.kind == TOKEN_IMPORT) 
        {
            if ( !parseImport() )
            {
                diag->error(report("Syntax error in import declaration!"));
                return false;
            }  
        } 
        else if (token.kind == TOKEN_MODULE) 
        {
            if ( !parseModule() )
            {
                diag->error(report("Syntax error in module declaration!")); 
                return false;
            } 
        }
        else if (token.kind == TOKEN_COMMENT_SINGLE || token.kind == TOKEN_MULTI_COMMENT) consume();
        else 
        {
            diag->error(report("Syntax error", "Unexpected symbol \'" + token.lexeme + "\'."));
            return false;
        }
    }

    return true;
}

ASTModule* Parser::getModule()
{
    return std::move(module);
}

bool Parser::parseModule()
{
    bool verdict = false;
    consume();
    Token token = peek(0);

    if ((token.kind != TOKEN_EOF && token.kind == TOKEN_IDENTIFIER) && peek().kind == TOKEN_SEMICOLON)
    {
        if (this->module->name.size() != 0)
        {
            diag->error(report("Syntax error", "Module redeclaration is not allowed in a compilation unit!"));
            return verdict;
        } 
        this->module->name = token.lexeme;
        verdict = true;
        consume(2);
    }
    return verdict;
}

bool Parser::parseImport()
{
    bool verdict = false;
    consume();
    Token token = peek(0);

    if ((token.kind != TOKEN_EOF && token.kind == TOKEN_IDENTIFIER) && peek().kind == TOKEN_SEMICOLON)
    {
        this->module->dependencies.insert(token.lexeme);
        verdict = true;
        consume(2);
    }
    return verdict;
}

ExprPtr Parser::parseExpression(uint8_t precedence, bool allowAssignment)
{
    Token token = advance();
    auto prefixIt = prefixParseFns.find(token.kind);
    
    if (prefixIt == prefixParseFns.end()) {
        diag->error(report("Unexpected token \'" + token.lexeme + "\'."));
        return nullptr;
    }

    ExprPtr left = prefixIt->second(*this, token);
    while (precedence < getPrecedence(peek(0).kind)) {
        token = advance();
        auto infixIt = infixParseFns.find(token.kind);
        if (infixIt == infixParseFns.end()) {
            break;
        }

        left = infixIt->second(*this, std::move(left), allowAssignment);
    }

    return left;
}

ExprPtr Parser::parseUnary(Token &token)
{
    TokenKind op = token.kind;
    ExprPtr expr = parseExpression();
    return std::make_shared<UnaryExpr>(op, std::move(expr));
}

ExprPtr Parser::parseNumber(Token &token) 
{
    try
    {
        int value = getNumericValue(token.lexeme);
        return std::make_shared<NumberExpr>(value);
    }
    catch(const std::exception& e)
    {
        diag->error(report("Invalid numeric format \'" + token.lexeme + "\'!", std::string{"Parsing failed with error: "} + e.what(), &token));
    }

    return nullptr;
}

ExprPtr Parser::parseString(Token &token)
{
    std::string value = "";

    if (token.lexeme.length() == 2) token.lexeme = "";
    else value = value.substr(1, value.length() - 2);

    return std::make_shared<StringExpr>(parseEscapeSequences(value).c_str());
}

ExprPtr Parser::parseChar(Token &token)
{
    std::string value ;
    if (token.lexeme.length() != 3 && token.lexeme.length() != 4) 
    {
        diag->error(report("Invalid character declaration!", "Only ASCII characters and escape sequences are currently supported!"));
    }
    else value = token.lexeme.substr(1, token.lexeme.length() - 2);
    
    return std::make_shared<CharExpr>(parseEscapeSequences(value).c_str()[0]);
}

ExprPtr Parser::parseIdentifier(Token &token) 
{
    return std::make_shared<VariableExpr>(token.lexeme);
}

ExprPtr Parser::parseGroup() 
{
    ExprPtr expr = parseExpression();
    
    if (peek(0).kind != TOKEN_RIGHT_PAREN) {
        diag->error(report("Missing symbol!", "Expected ')' character  at position"));
        return nullptr;
    }
    advance(); 
    return expr;
}

ExprPtr Parser::parseBinaryOp(ExprPtr left, bool /*allowAssignment*/) 
{
    TokenKind op = previous().kind;
    ExprPtr right = parseExpression(precedences[op]);
    return std::make_shared<BinaryExpr>(std::move(left), op, std::move(right));
}

ExprPtr Parser::parseAssignment(ExprPtr left, bool allowAssignment) 
{
    if (!allowAssignment)
    {
        diag->error(report("Syntax error", "Assignment is not allowed in expression!"));
        return nullptr;
    }
    TokenKind op = previous().kind;
    ExprPtr right = parseExpression(precedences[op], false);
    return std::make_shared<AssignmentExpr>(std::move(left), std::move(right));
}

ExprPtr Parser::parseFunctionCall(ExprPtr /*left*/, bool /*allowAssignment*/) {
    // TODO: implement function call node
    return nullptr;
}

ExprPtr Parser::parseInvocation(ExprPtr left, bool allowAssignment)
{
    TokenKind op = previous().kind;
    ExprPtr right = parseExpression(precedences[op], allowAssignment);
    return std::make_shared<InvocationExpr>(std::move(left), std::move(right));
}

ExprPtr Parser::parseResolution(std::string ns, bool allowAssignment)
{
    // TODO
    return nullptr;
}

ExprPtr Parser::parseIndexing(ExprPtr left, bool /*allowAssignment*/) {
    ExprPtr index = parseExpression(false);
    if (peek(0).kind != TOKEN_RIGHT_BRACKET) {
        diag->error(report("Missing symbol!", "Expected ']' character  at position"));
        return nullptr;;
    }
    consume(); // eat ']'
    return std::make_shared<IndexExpr>(std::move(left), std::move(index));
}

StmtPtr Parser::parseVarDeclaration()
{
    consume(); // Eat the VAR keyword
    Token token = advance();
    std::string var_name, raw_type;
    if ( TOKEN_IDENTIFIER != token.kind )
    {
        // TODO: Register name in scope. Also check for redeclaration + proper error handling
        diag->error(report("Syntax error", "Expected identifier name in variable declaration"));
    }
    var_name = token.lexeme;
    
    TypeInfo type = parseTypeInfo();
    
    if ( !match(TOKEN_EQUAL) )
    {
        diag->error(report("Missing symbol!", "Expected '=' character  at position"));
    }
    ExprPtr rhs = parseExpression(0, false);
    if ( !match(TOKEN_SEMICOLON) )
    {
        diag->error(report("Missing symbol!", "Expected ';' character  at position"));
    }
    
    return std::make_shared<VarDecStmt>(var_name, type, std::move(rhs));
}

TypeInfo Parser::parseTypeInfo()
{
    TypeInfo type;
    Token token = advance();
    if ( TOKEN_COLON != token.kind )
    {
        diag->error(report("Missing symbol!", "Expected ':' character  at position"));
    }
    token = advance();
    if ( !token.isOneOf(TOKEN_IDENTIFIER, 
                        TOKEN_I8, 
                        TOKEN_I16, 
                        TOKEN_I32, 
                        TOKEN_I64, 
                        TOKEN_FLOAT, 
                        TOKEN_CHAR, 
                        TOKEN_STRING, 
                        TOKEN_BOOL))
    {
        diag->error(report("Syntax error", "Expected type identifier before assignment!", &token));
    }

    type.type_name = token.lexeme;

    return type;
}

StmtPtr Parser::parseIfStatement()
{
    consume(); // Eat the IF keyword
    ExprPtr cond = parseExpression(0);
    StmtPtr then = parseStatement(); 

    if (!match(TOKEN_ELSE)) 
        return std::make_shared<IfStmt>(std::move(cond), std::move(then), nullptr);

    StmtPtr els = parseStatement();
    return std::make_shared<IfStmt>(std::move(cond), std::move(then), std::move(els));
}

StmtPtr Parser::parseBlockStatement()
{
    consume(); // Eat the '{'
    StmtList stmt_list;

    do
    {
        StmtPtr stmt = parseStatement();
        stmt_list.push_back(std::move(stmt));
    } while (!isEof() && peek(0).kind != TOKEN_RIGHT_CURLY);

    if (isEof())
    {
        diag->error(report("Missing symbol!", "Expected '}' character  at position"));
        return nullptr;
    }

    consume(); // Eat the '}'
    return std::make_shared<BlockStmt>(std::move(stmt_list));
}

StmtPtr Parser::parseStatement()
{
    // TODO: Implement the rest of the statement structures like ExprStatement, loops etc.
    switch (peek(0).kind)
    {
        case TOKEN_LEFT_CURLY:
            return parseBlockStatement();
        case TOKEN_VAR:
            return parseVarDeclaration();
        case TOKEN_IF:
            return parseIfStatement();
        default:
            std::cout << "Error parsing statements!" << std::endl;
            return nullptr;
    }
}

StmtPtr Parser::parseEntry()
{
    consume(); // Eat the entry keyword
    StmtPtr body = parseBlockStatement();
    return body;
}

void Parser::setDiag(Diagnostics *diagnostics)
{
    this->diag = diagnostics;
}

Message Parser::report(std::string message, std::string other_info, Token *token)
{
    return { .file = this->file->name, 
             .chunk = getCodeLine(file->tokens[parse_curr].location, file->code), 
             .loc = ( token == nullptr ) ? this->file->tokens[parse_curr].location : token->location, 
             .length = ( token == nullptr ) ? file->tokens[parse_curr].lexeme.length() : token->lexeme.length(), 
             .msg = message, 
             .other_info = other_info };
}

void Parser::registerPrefix(TokenKind kind, PrefixParseFn fn) {
    prefixParseFns[kind] = fn;
}

void Parser::registerInfix(TokenKind kind, InfixParseFn fn, uint8_t precedence) {
    infixParseFns[kind] = fn;
    precedences[kind] = precedence;
}

uint8_t Parser::getPrecedence(TokenKind kind) {
    auto it = precedences.find(kind);
    if (it != precedences.end()) {
        return it->second;
    }
    return 0;
}