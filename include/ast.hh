
#pragma once

#include <set>

#include "token.hh"
#include "util.hh"

class Expr {
public:
    virtual ~Expr() = default;
    virtual std::string print() = 0;
};

class Statement {
public:
    virtual ~Statement() = default;
    virtual std::string print() = 0;
};

using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Statement>;
using StmtList = std::vector<std::unique_ptr<Statement>>;

class NumberExpr : public Expr {
public:
    NumberExpr(int value) : value(value) {}
    std::string print() override { return "(" + std::to_string(value) + ")"; }
private:
    int value;
};

class StringExpr : public Expr {
public:
    StringExpr(const char* s) : str(s) {} 
    std::string print() override { return "(\"" + str + "\")"; }
private:
    std::string str;
};

class CharExpr : public Expr {
public:
    CharExpr(const char c) : ch(c) {}
    std::string print() override { return static_cast<std::string>("(\'") + (ch) + "\')"; } 
private:
    char ch;
};

class VariableExpr : public Expr {
public:
    VariableExpr(std::string name) : name(std::move(name)) {}
    std::string getName() { return this->name; }
    std::string print() override { return "(" + name + ")"; }
private:
    std::string name;
};

class BinaryExpr : public Expr {
public:
    BinaryExpr(ExprPtr left, TokenKind op, ExprPtr right)
        : left(std::move(left)), op(op), right(std::move(right)) {}
    std::string print() override { return "(" + left->print() + operatorToString(op) + right->print() + ")"; }
private:
    ExprPtr left;
    TokenKind op;
    ExprPtr right;
};

class AssignmentExpr : public Expr {
public:
    AssignmentExpr(ExprPtr lhs, ExprPtr rhs)
        : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    std::string print() override { return "(" + lhs->print() + "=" + rhs->print() + ")"; }
private:
    ExprPtr lhs;
    ExprPtr rhs;
};

class UnaryExpr : public Expr {
public:
    UnaryExpr(TokenKind op, ExprPtr expr) : op(op), expr(std::move(expr)) {}
    std::string print() override { return "(" + operatorToString(op) + expr->print() + ")"; }
private:
    TokenKind op;
    ExprPtr expr;
};

class FunctionCallExpr : public Expr {
public:
    FunctionCallExpr(std::string name, std::vector<ExprPtr> args)
        : name(std::move(name)), args(std::move(args)) {}
    std::string print() override { return "FUNC CALL"; } // TODO: print func call properly
private:
    std::string name;
    std::vector<ExprPtr> args;
};

class IndexExpr : public Expr {
public:
    IndexExpr(ExprPtr array, ExprPtr index)
        : array(std::move(array)), index(std::move(index)) {}
    std::string print() override { return "indexing..."; } // TODO: print indexing properly
private:
    ExprPtr array;
    ExprPtr index;
};

class VarDecStmt : public Statement {
public:
    VarDecStmt(std::string name, TypeInfo type, ExprPtr lhs) : name(name), type(type), lhs(std::move(lhs)) {}
    std::string print() override { return "[" + name + " : " + type.type_name + " = " + lhs->print() + "]"; }
private:
    std::string name;
    TypeInfo type;
    ExprPtr lhs;
};

class IfStmt : public Statement {
public:
    IfStmt(ExprPtr cond, StmtPtr then, StmtPtr els) : cond(std::move(cond)), then(std::move(then)), els(std::move(els)) {}
    std::string print() override { return "IF [ " + cond->print() + " ] then { " + (then != nullptr ? then->print() : "") + " } else { " + (els != nullptr ? els->print() : "") + " }"; }
private:
    ExprPtr cond; 
    StmtPtr then; // TODO: This should be a block expression
    StmtPtr els; // TODO: This should be a block expression
};

class BlockStmt : public Statement {
public:
    BlockStmt(StmtList statements) : statements(std::move(statements)) {}
    std::string print() override 
    {
        std::string result = "BLOCK [ ";
        for (auto &stmt : statements)
        {
            result = result + stmt->print() + " ";
        }
        result = result + "]";
        return result;
    }
private:
    StmtList statements;
};

class ASTModule {
public:
    std::string module_name;
    std::set<std::string> dependencies;
    StmtPtr entry;
};