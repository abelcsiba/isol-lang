
#include <iostream>
#include <cstddef>

#include "parser.hh"

Parser::Parser(std::vector<Token> tokens)
{
    this->token_list = tokens;
    this->parse_curr = 0;
}

bool Parser::isEof()
{
    return this->token_list.at(this->parse_curr).kind == TOKEN_EOF;
}

void Parser::advance(size_t offset)
{
    this->parse_curr += offset;
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
    return this->token_list.at(this->parse_curr + offset);
}

bool Parser::parse()
{
    this->module = new ASTModule();
    while (!isEof())
    {
        Token token = peek(0);

        if (token.kind == TOKEN_PURE) std::cout << "Parsing pure func." << std::endl;
        else if (token.kind == TOKEN_RECORD) std::cout << "Parsing record." << std::endl;
        else if (token.kind == TOKEN_ENTITY) std::cout << "Parsing entity." << std::endl;
        else if (token.kind == TOKEN_ENTRY) std::cout << "Parsing entry." << std::endl;
        else if (token.kind == TOKEN_IMPORT) std::cout << "Parsing import." << std::endl;
        else if (token.kind == TOKEN_MODULE) 
        {
            bool verdict = parseModule();
            if (!verdict)
            {
                std::cout << "Incorrect module declaration!" << std::endl;
                return false;
            }
        }
        else if (token.kind == TOKEN_COMMENT_SINGLE || token.kind == TOKEN_MULTI_COMMENT) {}
        else 
        {
            // TODO: Proper logging here, please
            std::cout << "Unexpected symbol \'" << token.lexeme << "\'." << std::endl;
            return false;
        }
        advance();
    }
    // TODO: fix return
    return true;
}

ASTModule* Parser::getModule()
{
    return this->module;
}

bool Parser::parseModule()
{
    bool verdict = false;
    Token token = peek(1);
    if ((token.kind != TOKEN_EOF && token.kind == TOKEN_IDENTIFIER) && peek(2).kind == TOKEN_SEMICOLON)
    {
        if (this->module->module_name.size() != 0)
        {
            std::cout << "Module redeclaration!" << std::endl;
            return verdict;
        } 
        this->module->module_name = token.lexeme;
        verdict = true;
        advance(2);
    }
    return verdict;
}

ASTExpression Parser::parseExpression()
{
    ASTExpression expr;
    return expr;
}