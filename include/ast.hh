
#pragma once

#include <set>

#include "token.hh"

class Expr {
public:
    virtual ~Expr() = default;
};

using ExprPtr = std::unique_ptr<Expr>;

class NumberExpr : public Expr {
public:
    NumberExpr(int value) : value(value) {}
private:
    int value;
};

class VariableExpr : public Expr {
public:
    VariableExpr(std::string name) : name(std::move(name)) {}
    std::string getName() { return this->name; }
private:
    std::string name;
};

class BinaryExpr : public Expr {
public:
    BinaryExpr(ExprPtr left, TokenKind op, ExprPtr right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

private:
    ExprPtr left;
    TokenKind op;
    ExprPtr right;
};

class AssignmentExpr : public Expr {
public:
    AssignmentExpr(std::string name, ExprPtr value)
        : name(std::move(name)), value(std::move(value)) {}

private:
    std::string name;
    ExprPtr value;
};

class FunctionCallExpr : public Expr {
public:
    FunctionCallExpr(std::string name, std::vector<ExprPtr> args)
        : name(std::move(name)), args(std::move(args)) {}

private:
    std::string name;
    std::vector<ExprPtr> args;
};

class IndexExpr : public Expr {
public:
    IndexExpr(ExprPtr array, ExprPtr index)
        : array(std::move(array)), index(std::move(index)) {}

private:
    ExprPtr array;
    ExprPtr index;
};

class ASTModule {
public:
    std::string module_name;
    std::set<std::string> dependencies;
};