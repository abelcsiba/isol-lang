
#include <iostream>
#include <cstddef>

#include "parser.hh"


Parser::Parser(TokenList tokens)
{
    this->token_list = tokens;
    this->parse_curr = 0;

    registerPrefix(TOKEN_NUM_LITERAL, [](Parser& p, Token &prev) { return p.parseNumber(prev); });
    registerPrefix(TOKEN_IDENTIFIER, [](Parser& p, Token &prev) { return p.parseIdentifier(prev); }); 
    registerPrefix(TOKEN_LEFT_PAREN, [](Parser& p, Token &prev) { return p.parseGroup(prev); }); // TODO: do I need prev here?
    registerPrefix(TOKEN_MINUS, [](Parser& p, Token &prev) { return p.parseUnary(prev); });
    registerPrefix(TOKEN_PLUS, [](Parser& p, Token &prev) { return p.parseUnary(prev); });

    registerInfix(TOKEN_PLUS, [](Parser& p, ExprPtr left) { return p.parseBinaryOp(std::move(left)); }, 10);
    registerInfix(TOKEN_MINUS, [](Parser& p, ExprPtr left) { return p.parseBinaryOp(std::move(left)); }, 10);
    registerInfix(TOKEN_STAR, [](Parser& p, ExprPtr left) { return p.parseBinaryOp(std::move(left)); }, 20);
    registerInfix(TOKEN_SLASH, [](Parser& p, ExprPtr left) { return p.parseBinaryOp(std::move(left)); }, 20);
    registerInfix(TOKEN_LEFT_PAREN, [](Parser& p, ExprPtr left) { return p.parseFunctionCall(std::move(left)); }, 30);
    registerInfix(TOKEN_LEFT_BRACE, [](Parser& p, ExprPtr left) { return p.parseIndexing(std::move(left)); }, 30);
    registerInfix(TOKEN_EQUAL, [](Parser& p, ExprPtr left) { return p.parseAssignment(); }, 5);
}

bool Parser::isEof()
{
    return token_list[this->parse_curr].kind == TOKEN_EOF;
}

Token Parser::advance()
{
    size_t current = parse_curr;
    parse_curr++;
    return token_list[current];
}

void Parser::consume(size_t offset)
{
    parse_curr += offset;
}

Token Parser::peek(size_t offset)
{

    if (this->parse_curr + offset >= this->token_list.size()) 
    {
        // TODO: Add proper error handling
        Token token;
        token.kind = TOKEN_EOF;
        return token;
    }
    return token_list[this->parse_curr + offset];
}

Token Parser::previous()
{
    return token_list[parse_curr - 1]; 
}

bool Parser::match(TokenKind kind)
{
    return peek(0).kind == kind;
}

bool Parser::parse()
{
    this->module = new ASTModule();
    while (!isEof())
    {
        Token token = peek(0);

        /*if ( TOKEN_VAR == token.kind ) 
        {
            StmtPtr stmt = parseVarDeclaration();
            std::cout << "STATEMENT: " << stmt->print() << std::endl;
        }
        else*/ if ( TOKEN_IF == token.kind )
        {
            consume();
            StmtPtr stmt = parseIfStatement();
            std::cout << "STATEMENT: " << stmt->print() << std::endl;
        }
        /*if (peek(0).kind == TOKEN_SEMICOLON)
            std::cout << "Expression end\n";
        ExprPtr expr = parseExpression(0);
        if ( expr == nullptr ) 
        {
            std::cout << "Failed to parse expression!" << std::endl;
            return false;
        }
        std::cout << "Expression: " << expr->print() << std::endl;*/
        
        /*Token token = advance();

        if (token.kind == TOKEN_PURE) std::cout << "Parsing pure func." << std::endl;
        else if (token.kind == TOKEN_RECORD) std::cout << "Parsing record." << std::endl;
        else if (token.kind == TOKEN_ENTITY) std::cout << "Parsing entity." << std::endl;
        else if (token.kind == TOKEN_ENTRY) std::cout << "Parsing entry." << std::endl;
        else if (token.kind == TOKEN_IMPORT) { if ( !parseImport() ) return false;  } // TODO: add error logging here
        else if (token.kind == TOKEN_MODULE) { if ( !parseModule() ) return false; } // TODO: add error logging here
        else if (token.kind == TOKEN_COMMENT_SINGLE || token.kind == TOKEN_MULTI_COMMENT) {}
        else 
        {
            // TODO: Proper logging here, please
            std::cout << "Unexpected symbol \'" << token.lexeme << "\'." << std::endl;
            return false;
        }
        //advance();*/
    }
    // TODO: fix return
    return true;
}

ASTModule* Parser::getModule()
{
    return std::move(module);
}

bool Parser::parseModule()
{
    bool verdict = false;
    Token token = peek(0);
    if ((token.kind != TOKEN_EOF && token.kind == TOKEN_IDENTIFIER) && peek().kind == TOKEN_SEMICOLON)
    {
        if (module->module_name.size() != 0)
        {
            // TODO: This should be removed later, once there is error logging
            std::cout << "Module redeclaration!" << std::endl;
            return verdict;
        } 
        module->module_name = token.lexeme;
        verdict = true;
        consume(2);
    }
    return verdict;
}

bool Parser::parseImport()
{
    bool verdict = false;
    Token token = peek(0);
    if ((token.kind != TOKEN_EOF && token.kind == TOKEN_IDENTIFIER) && peek().kind == TOKEN_SEMICOLON)
    {
        this->module->dependencies.insert(token.lexeme);
        verdict = true;
        consume(2);
    }
    return verdict;
}

ExprPtr Parser::parseExpression(uint8_t precedence)
{
    Token token = advance();
    auto prefixIt = prefixParseFns.find(token.kind);
    
    if (prefixIt == prefixParseFns.end()) {
        std::cout << "Unexpected token \'" << token.lexeme << "\'." << std::endl;
        return nullptr;
    }

    ExprPtr left = prefixIt->second(*this, token);
    while (precedence < getPrecedence(peek(0).kind)) {
        token = advance();
        auto infixIt = infixParseFns.find(token.kind);
        if (infixIt == infixParseFns.end()) {
            break;
        }

        left = infixIt->second(*this, std::move(left));
    }

    return left;
}

ExprPtr Parser::parseUnary(Token &token)
{
    TokenKind op = token.kind;
    ExprPtr expr = parseExpression();
    return std::make_unique<UnaryExpr>(op, std::move(expr));
}

ExprPtr Parser::parseNumber(Token &token) 
{
    try
    {
        int value = std::stoi(token.lexeme);
        return std::make_unique<NumberExpr>(value);
    }
    catch(const std::exception& e)
    {
        // TODO: Implement proper number parsing the remove the try/catch shit
        std::cerr << "FAILED to parse " << token.lexeme << " " << e.what() << '\n';
    }

    return nullptr;
}

ExprPtr Parser::parseIdentifier(Token &token) 
{
    std::string name = token.lexeme;
    if (token.kind == TOKEN_EQUAL) {
        advance();
        ExprPtr value = parseExpression();
        return std::make_unique<AssignmentExpr>(name, std::move(value));
    } else {
        return std::make_unique<VariableExpr>(name);
    }
}

ExprPtr Parser::parseGroup(Token &token) 
{
    ExprPtr expr = parseExpression();
    
    if (peek(0).kind != TOKEN_RIGHT_PAREN) {
        std::cout << "Expected ')'" << std::endl;
        return nullptr;
    }
    advance(); 
    return expr;
}

ExprPtr Parser::parseBinaryOp(ExprPtr left) 
{
    TokenKind op = previous().kind;
    ExprPtr right = parseExpression(precedences[op]);
    return std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
}

ExprPtr Parser::parseAssignment() 
{
    std::string name = peek(0).lexeme;
    advance();
    ExprPtr value = parseExpression();
    return std::make_unique<AssignmentExpr>(name, std::move(value));
}

ExprPtr Parser::parseFunctionCall(ExprPtr left) {
    /*std::vector<ExprPtr> args;
    if (peek(0).kind != TOKEN_RIGHT_PAREN) {
        do {
            args.push_back(parseExpression());
        } while (match(TokenType::COMMA) && advance().type != TokenType::RPAREN);
    }
    if (peek(0) != TOKEN_RIGHT_PAREN) {
        return nullptr;
    }
    advance(); // consume ')'
    return std::make_unique<FunctionCallExpr>(static_cast<VariableExpr*>(left.get())->getName(), std::move(args));*/
    return nullptr;
}

ExprPtr Parser::parseIndexing(ExprPtr left) {
    ExprPtr index = parseExpression();
    if (peek(0).kind != TOKEN_RIGHT_BRACE) {
        return nullptr;;
    }
    advance(); // consume ']'
    return std::make_unique<IndexExpr>(std::move(left), std::move(index));
}

// TODO: Parsing type should be in a different function
StmtPtr Parser::parseVarDeclaration()
{
    consume(); // Eat the VAR keyword
    Token token = advance();
    std::string var_name, raw_type;
    if ( TOKEN_IDENTIFIER != token.kind )
    {
        // TODO: Register name in scope. Also check for redeclaration + proper error handling
        std::cout << "Expected identifier name!" << std::endl;
        return nullptr;
    }
    var_name = token.lexeme;
    token = advance();
    if ( TOKEN_COLON != token.kind )
    {
        // TODO: proper error handling here, please
        std::cout << "Expected symbol \':\'!" << std::endl;
        return nullptr;
    }
    token = advance();
    if ( TOKEN_IDENTIFIER != token.kind && TOKEN_I8 != token.kind )
    {
        // TODO: proper error handling
        std::cout << "Expected type identifier!" << std::endl;
        return nullptr;
    }
    raw_type = token.lexeme;
    token = advance();
    if ( TOKEN_EQUAL != token.kind )
    {
        // TODO: proper error handling here, please
        std::cout << "Expected symbol \'=\'!" << std::endl;
        return nullptr;
    }
    ExprPtr lhs = parseExpression(0);
    token = advance();
    if ( TOKEN_SEMICOLON != token.kind )
    {
        // TODO: proper error handling here, please
        std::cout << "Expected symbol \';\'!" << std::endl;
        return nullptr;
    }
    return std::make_unique<VarDecStmt>(var_name, raw_type, std::move(lhs));
}

StmtPtr Parser::parseIfStatement()
{
    ExprPtr cond = parseExpression(0);
    StmtPtr then = parseVarDeclaration();
    consume(); // I only assume here, it is an else token... fix it!
    StmtPtr els = parseVarDeclaration();
    return std::make_unique<IfStmt>(std::move(cond), std::move(then), std::move(els));
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