
#include "token.hh"


void newToken(Token &token, const int col, const int row, TokenKind kind, TokenErrorCode err)
{
    token.kind = kind;
    token.location = { .col = col, .row = row };
    token.err = err;
}
