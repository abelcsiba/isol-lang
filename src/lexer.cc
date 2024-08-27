
#include <iostream>
#include <cstring>
#include "lexer.hh"
#include "consts.hh"
#include <unistd.h>

Lexer::Lexer(const char* raw_code, CodeFile file)
{
    this->code = raw_code;
    this->lex_begin = 0;
    this->lex_curr = 0;
    this->loc.col = 0;
    this->loc.row = 0;
    this->file = file;
}

// ----- Helpers -----

bool Lexer::isEof() 
{
    return ( lex_curr == this->code.length());
}

bool Lexer::isWhitespace()
{
    if (lex_curr >= this->code.length()) return false;
    char cur = this->code.at(lex_curr);
    return ( cur == '\n' || cur == '\r' || cur == '\t' || cur == ' ' );
}

bool Lexer::isDigit()
{
    if (lex_curr >= this->code.length()) return false;
    char cur = this->code.at(lex_curr);
    return ( cur >= '0' && cur <= '9' );
}

bool Lexer::isShadower()
{
    if (lex_curr >= this->code.length()) return false;
    char cur = this->code.at(lex_curr);
    return cur == '_';
}

bool Lexer::isAlpha()
{
    if (lex_curr >= this->code.length()) return false;
    char cur = this->code.at(lex_curr);
    return ( ( cur >= 'a' && cur <= 'z') || ( cur >= 'A' && cur <= 'Z' ) );
}

bool Lexer::isAlphaNumeric()
{
    return isAlpha() || isDigit();
}

// ----- Tokenizers -----

bool Lexer::adjustPos(int offset)
{
    this->lex_curr += offset;
    this->loc.col += offset;
    return true;
}

void Lexer::advance(int offset)
{
    for (int i = 0; i < offset; i++)
    {
        if (this->code.at(this->lex_curr + i) == '\n')
        {
            this->loc.col = 0;
            this->loc.row++;
        }
        else
        {
            this->loc.col++;
        }
    }

    this->lex_curr += offset;
}

const char Lexer::peek(int offset)
{
    if (lex_curr + offset >= this->code.length()) return '#';
    return this->code.at(lex_curr + offset);
}

void Lexer::eatMultilineComment(Token &token)
{
    Location saved_loc = { .col = this->loc.col, .row = this->loc.row };
    char c;
    char c1;
    advance(2);
    bool found = false;
    while (!isEof() && !found)
    {
        c = peek(0);
        c1 = peek(1);

        if ( c == '*' && c1 == '/')
            found = true;
        else
            advance();
    }
    if (found)
    {
        token = consume(TOKEN_MULTI_COMMENT, 2);
    }
    else
    {
        token = consume(TOKEN_ERROR);
    }
    token.location = saved_loc;
}

void Lexer::eatSinglelineComment(Token &token)
{
    do 
    { 
        advance(); 
    } while (peek(0) != '\n'); 
    token = consume(TOKEN_COMMENT_SINGLE, 1); 
    this->loc.row += 1;
    this->loc.col = 0;
    // 
}

void Lexer::eatIdentifier(Token &token)
{
    token.kind = TOKEN_IDENTIFIER;
    if (isShadower()) 
    {
        token.shadower = true;
        lex_begin += 1;
        advance();
    }

    while (isAlphaNumeric() || isShadower())
    {
        advance();
    }

    matchKeywords(token); 
}

void Lexer::eatNumber(Token &token)
{
    while (isDigit()) 
    {
        advance();
    }
    token = consume(TOKEN_NUM_LITERAL);
}

Token Lexer::consume(TokenKind kind, int offset)
{
    adjustPos(offset);
    Token token;
    size_t lexeme_size = 0;
    if (lex_curr == this->code.length())
    {
        token.lexeme = new char[1];
        lexeme_size = 0;
        std::memset(token.lexeme, '\0', 1);
    }
    else
    {
        lexeme_size = this->lex_curr - this->lex_begin;
        if (kind == TOKEN_COMMENT_SINGLE) lexeme_size -= 1;

        token.lexeme = new char[lexeme_size + 1];
        std::memset(token.lexeme, '\0', lexeme_size + 1);
        this->code.copy(token.lexeme, lexeme_size, this->lex_begin);
    }
    //std::cout << "Consume: " << lex_curr << " " << lex_begin << " lexeme: " << token.lexeme << std::endl;
    Location token_location;

    token_location.col = this->loc.col - lexeme_size;
    token_location.row = this->loc.row;

    token.kind = kind;
    token.location = token_location;

    return token;
}

bool Lexer::match(const char *keyword)
{
    size_t lexeme_size = this->lex_curr - this->lex_begin - 1;
    if (std::memcmp(keyword, this->code.substr(this->lex_begin, lexeme_size).data(), strlen(keyword)) == 0)
        return true;

    return false;
}

void Lexer::matchKeywords(Token &token)
{
    if (match(KW::IMPORT)) token = consume(TOKEN_IMPORT);
    else if (match(KW::MODULE)) token = consume(TOKEN_MODULE);
    else if (match(KW::RECORD)) token = consume(TOKEN_RECORD);
    else if (match(KW::PURE)) token = consume(TOKEN_PURE);
    else if (match(KW::RETURN)) token = consume(TOKEN_RETURN);
    else if (match(KW::ENTITY)) token = consume(TOKEN_ENTITY);
    else if (match(KW::VAR)) token = consume(TOKEN_VAR);
    else if (match(KW::ENTRY)) token = consume(TOKEN_ENTRY);
    else if (match(KW::PROC)) token = consume(TOKEN_PROC);
    else if (match(KW::IF)) token = consume(TOKEN_IF);
    else if (match(KW::ELSE)) token = consume(TOKEN_ELSE);
    else if (match(KW::FOR)) token = consume(TOKEN_FOR);
    else if (match(KW::WHILE)) token = consume(TOKEN_WHILE);
    else if (match(KW::THIS)) token = consume(TOKEN_THIS);
    else if (match(KW::TRUE)) token = consume(TOKEN_TRUE);
    else if (match(KW::FALSE)) token = consume(TOKEN_FALSE);
    else if (match(KW::I8)) token = consume(TOKEN_I8);
    else if (match(KW::I16)) token = consume(TOKEN_I16);
    else if (match(KW::I32)) token = consume(TOKEN_I32);
    else if (match(KW::I64)) token = consume(TOKEN_I64);
    else if (match(KW::CHAR)) token = consume(TOKEN_CHAR);
    else if (match(KW::FLOAT)) token = consume(TOKEN_FLOAT);
    else if (match(KW::DOUBLE)) token = consume(TOKEN_DOUBLE);
    else if (match(KW::BOOL)) token = consume(TOKEN_BOOL);
    else if (match(KW::STR)) token = consume(TOKEN_STRING);
    else token = consume(TOKEN_IDENTIFIER);
}

Token Lexer::nextToken()
{
    while (isWhitespace() && !isEof())
    {
        lex_begin += 1;
        advance();
    }

    Token token;

    auto c = peek(0);
    auto c1 = peek(1);

    if ( c == '+' && c1 == '=' ) token = consume(TOKEN_PLUS_EQUAL, 2);
    else if ( c == '+' && c1 == '+' ) token = consume(TOKEN_PLUS_PLUS, 2);
    else if ( c == '-' && c1 == '=' ) token = consume(TOKEN_MINUS_EQUAL, 2);
    else if ( c == '-' && c1 == '-' ) token = consume(TOKEN_MINUS_MINUS, 2);
    else if ( c == '/' && c1 == '=' ) token = consume(TOKEN_SLASH_EQUAL, 2);
    else if ( c == '*' && c1 == '=' ) token = consume(TOKEN_STAR_EQUAL, 2);
    else if ( c == '<' && c1 == '=' ) token = consume(TOKEN_LESS_EQUAL, 2);
    else if ( c == '>' && c1 == '=' ) token = consume(TOKEN_GREATER_EQUAL, 2);
    else if ( c == '=' && c1 == '=' ) token = consume(TOKEN_EQUAL_EQUAL, 2);
    else if ( c == '!' && c1 == '=' ) token = consume(TOKEN_BANG_EQUAL, 2);
    else if ( c == '-' && c1 == '>' ) token = consume(TOKEN_ARROW, 2);
    else if ( c == '<' && c1 == '-' ) token = consume(TOKEN_GENERATOR, 2);
    else if ( c == '/' && c1 == '/' ) eatSinglelineComment(token);
    else if ( c == '/' && c1 == '*' ) eatMultilineComment(token);
    else if ( c == '+') token = consume(TOKEN_PLUS, 1);
    else if ( c == '-' ) token = consume(TOKEN_MINUS, 1);
    else if ( c == '/' ) token = consume(TOKEN_SLASH, 1);
    else if ( c == '*' ) token = consume(TOKEN_STAR, 1);
    else if ( c == '(' ) token = consume(TOKEN_LEFT_PAREN, 1);
    else if ( c == ')' ) token = consume(TOKEN_RIGHT_PAREN, 1);
    else if ( c == '[' ) token = consume(TOKEN_LEFT_BRACKET, 1);
    else if ( c == ']' ) token = consume(TOKEN_RIGHT_BRACKET, 1);
    else if ( c == '{' ) token = consume(TOKEN_LEFT_BRACE, 1);
    else if ( c == '}' ) token = consume(TOKEN_RIGHT_BRACE, 1);
    else if ( c == ',' ) token = consume(TOKEN_COMMA, 1);
    else if ( c == ';' ) token = consume(TOKEN_SEMICOLON, 1);
    else if ( c == '=' ) token = consume(TOKEN_EQUAL, 1);
    else if ( c == '<' ) token = consume(TOKEN_LESS, 1);
    else if ( c == '>' ) token = consume(TOKEN_GREATER, 1);
    else if ( c == '.' ) token = consume(TOKEN_DOT, 1);
    else if ( c == ':' ) token = consume(TOKEN_COLON, 1);
    else if ( c == '!' ) token = consume(TOKEN_BANG, 1);
    else if (isAlpha() || isShadower()) eatIdentifier(token);
    else if (isDigit()) eatNumber(token);
    else if (lex_curr == this->code.length()) 
    {
        token = consume(TOKEN_EOF);
    }
    else 
    { 
        token = consume(TOKEN_ERROR, 1);
        token.err = UNRECOGNIZED_SYMBOL;
    }

    return token;
}

void Lexer::addEofToken()
{
    Token token;
    Location loc;
    loc.col = this->loc.col;
    loc.row = this->loc.row;
    token.location = loc;
    token.kind = TOKEN_EOF;
    token_list.push_back(token);
}

bool Lexer::lex() 
{
    bool verdict = true;

    if (isEof()) {
        Token token;
        newToken(token, -1, -1, TOKEN_ERROR, BAD_INPUT_FILE);
        return false;
    }

    while (!isEof())
    {
        this->lex_begin = this->lex_curr;
        Token token = nextToken();
        token_list.push_back(token);
        if (token.kind == TOKEN_ERROR) 
        {
            return false;
        }
    }

    if (token_list.back().kind != TOKEN_EOF)
        addEofToken();

    return verdict;
}

std::vector<Token> Lexer::getTokens()
{
    return token_list;
}
