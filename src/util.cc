
#include  <iostream>

#include "util.hh"
#include <string.h>

std::string tokenKindToString(TokenKind kind)
{
    switch (kind)
    {
        case TOKEN_ARROW:
            return "TOKEN_ARROW";
        case TOKEN_BANG:
            return "TOKEN_BANG";
        case TOKEN_BANG_EQUAL:
            return "TOKEN_BANG_EQUAL";
        case TOKEN_BOOL:
            return "TOKEN_BOOL";
        case TOKEN_CHAR:
            return "TOKEN_CHAR";
        case TOKEN_CHAR_LITERAL:
            return "TOKEN_CHAR_LITERAL";
        case TOKEN_COLON:
            return "TOKEN_COLON";
        case TOKEN_COMMA:
            return "TOKEN_COMMA";
        case TOKEN_COMMENT_SINGLE:
            return "TOKEN_COMMENT_SINGLE";
        case TOKEN_DOT:
            return "TOKEN_DOT";
        case TOKEN_DOUBLE:
            return "TOKEN_DOUBLE";
        case TOKEN_ELSE:
            return "TOKEN_ELSE";
        case TOKEN_ENTITY:
            return "TOKEN_ENTITY";
        case TOKEN_ENTRY:
            return "TOKEN_ENTRY";
        case TOKEN_EOF:
            return "TOKEN_EOF";
        case TOKEN_EQUAL:
            return "TOKEN_EQUAL";
        case TOKEN_EQUAL_EQUAL:
            return "TOKEN_EQUAL_EQUAL";
        case TOKEN_ERROR:
            return "TOKEN_ERROR";
        case TOKEN_FALSE:
            return "TOKEN_FALSE";
        case TOKEN_FLOAT:
            return "TOKEN_FLOAT";
        case TOKEN_FOR:
            return "TOKEN_FOR";
        case TOKEN_GENERATOR:
            return "TOKEN_GENERATOR";
        case TOKEN_GREATER:
            return "TOKEN_GREATER";
        case TOKEN_GREATER_EQUAL:
            return "TOKEN_GREATER_EQUAL";
        case TOKEN_I16:
            return "TOKEN_I16";
        case TOKEN_I32:
            return "TOKEN_I32";
        case TOKEN_I64:
            return "TOKEN_I64";
        case TOKEN_I8:
            return "TOKEN_I8";
        case TOKEN_IDENTIFIER:
            return "TOKEN_IDENTIFIER";
        case TOKEN_IF:
            return "TOKEN_IF";
        case TOKEN_IMPORT:
            return "TOKEN_IMPORT";
        case TOKEN_LEFT_CURLY:
            return "TOKEN_LEFT_CURLY";
        case TOKEN_LEFT_BRACKET:
            return "TOKEN_LEFT_BRACKET";
        case TOKEN_LEFT_PAREN:
            return "TOKEN_LEFT_PAREN";
        case TOKEN_LESS:
            return "TOKEN_LESS";
        case TOKEN_LESS_EQUAL:
            return "TOKEN_LESS_EQUAL";
        case TOKEN_MINUS:
            return "TOKEN_MINUS";
        case TOKEN_MINUS_EQUAL:
            return "TOKEN_MINUS_EQUAL";
        case TOKEN_MINUS_MINUS:
            return "TOKEN_MINUS_MINUS";
        case TOKEN_MODULE:
            return "TOKEN_MODULE";
        case TOKEN_MULTI_COMMENT:
            return "TOKEN_MULTI_COMMENT";
        case TOKEN_NUM_LITERAL:
            return "TOKEN_NUM_LITERAL";
        case TOKEN_PIPE_PIPE:
            return "TOKEN_PIPE_PIPE";
        case TOKEN_PLUS:
            return "TOKEN_PLUS";
        case TOKEN_PLUS_EQUAL:
            return "TOKEN_PLUS_EQUAL";
        case TOKEN_PLUS_PLUS:
            return "TOKEN_PLUS_PLUS";
        case TOKEN_PROC:
            return "TOKEN_PROC";
        case TOKEN_PURE:
            return "TOKEN_PURE";
        case TOKEN_RECORD:
            return "TOKEN_RECORD";
        case TOKEN_RETURN:
            return "TOKEN_RETURN";
        case TOKEN_RIGHT_CURLY:
            return "TOKEN_RIGHT_CURLY";
        case TOKEN_RIGHT_BRACKET:
            return "TOKEN_RIGHT_BRACKET";
        case TOKEN_RIGHT_PAREN:
            return "TOKEN_RIGHT_PAREN";
        case TOKEN_SEMICOLON:
            return "TOKEN_SEMICOLON";
        case TOKEN_SLASH:
            return "TOKEN_SLASH";
        case TOKEN_SLASH_EQUAL:
            return "TOKEN_SLASH_EQUAL";
        case TOKEN_STAR:
            return "TOKEN_STAR";
        case TOKEN_STAR_EQUAL:
            return "TOKEN_STAR_EQUAL";
        case TOKEN_STRING:
            return "TOKEN_STRING";
        case TOKEN_STRING_LITERAL:
            return "TOKEN_STRING_LITERAL";
        case TOKEN_THIS:
            return "TOKEN_THIS";
        case TOKEN_TRUE:
            return "TOKEN_TRUE";
        case TOKEN_VAR:
            return "TOKEN_VAR";
        case TOKEN_WHILE:
            return "TOKEN_WHILE";
        default:
            return "TOKEN_UNKNOWN";
    }
}

void prettyPrintToken(Token token) 
{
    std::cout << "[Token: " << tokenKindToString(token.kind) << " lexeme: " << ((token.kind != TOKEN_EOF) ? token.lexeme : "EOF") << " Pos: " << token.location.row << ":" << token.location.col << "]" << std::endl;
}

bool isValidNumber(std::string base, const char* c)
{
    try
    {
        int b = std::stoi(base);
        if (b > 16)
        {
            return false;
        }
        else if (b <=16 && b > 10)
        {
            for (size_t i = 0; i < strlen(c); i++)
            if (! (( c[i] >= '0' && c[i] < ('0' + b)) || (c[i] >= 'A' && c[i] < ('A' + b - 10))))
            {
                return false;
            }
        }
        else if (b <= 10 && b >= 2)
        {
            for (size_t i = 0; i < strlen(c); i++)
            if (!( c[i] >= '0' && c[i] < ('0' + b)))
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    catch(const std::exception& e)
    {
        return false;
    }
    
    return true;
}

std::string operatorToString(TokenKind op)
{
    switch (op)
    {
    case TOKEN_PLUS:
        return "+";
    case TOKEN_MINUS:
        return "-";
    case TOKEN_STAR:
        return "*";
    case TOKEN_SLASH:
        return "/";
    case TOKEN_NUM_LITERAL:
        return "NUM";
    case TOKEN_IDENTIFIER:
        return "ID";
    case TOKEN_AND:
        return "&";
    default:
        return "(null)";
    }
}

int64_t getNumericValue(std::string num)
{
    auto it = num.find('#');
    uint8_t base = 10;
    size_t start = 0;
    if (it != std::string::npos)
    {
        start = it + 1;
        base = std::stoi(num.substr(0, it));
    }

    std::string val = num.substr(start, num.length() - start);
    if (val.length() == 0)
        throw std::runtime_error("Missing numeric literal after base specification");
    
    // TODO: add error handling here. What if num cannot be converted?
    return std::stoi(num.substr(start, num.length() - start), nullptr, base);
}

std::string parseEscapeSequences(std::string_view data)
{
    std::string value = "";
    char c, c1;
    for (size_t i = 0; i < data.length() - 1; i++)
    {
        c = data[i];
        c1 = data[i + 1];
        if (c == '\\')
        {
            if (c1 == 'n') value += '\n';
            else if (c1 == 't') value += '\t';
            else if (c1 == 'r') value += '\r';
            else if (c1 == 'a') value += '\a';
            else if (c1 == 'b') value += '\b';
            else if (c1 == 'e') value += '\e';
            else if (c1 == 'v') value += '\v';
            else if (c1 == 'f') value += '\f';
            else if (c1 == '\\') value += '\\';
            else if (c1 == '\'') value += '\'';
            else if (c1 == '\"') value += '\"';
            else value += c;
        } else value += c;
    }
    return value;
}

std::string getCodeLine(Location loc, char* const code)
{
    size_t line_num = 1, index = 0;
    size_t length = strlen(code);

    while (line_num != static_cast<size_t>(loc.row) && index < length)
        if (code[index++] == '\n')
            line_num++;

    size_t begin = index;

    while (index < length && code[index] != '\n')
        index++;

    std::string line = "";
    for (size_t i = begin; i < index; i++)
        line += code[i];

    return line;    
}

