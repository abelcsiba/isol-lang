
#pragma once

#include <string>
#include <set>

class ASTDeclaration {

};

class ASTPureDeclaration : public ASTDeclaration {

};

class ASTEntry : public ASTDeclaration {

};

class ASTRecordDeclaration : ASTDeclaration {

};

class ASTEntityDeclaration : ASTDeclaration {

};

class ASTStatement {

};

class ASTExpression {

};

class ASTModule {
public:
    std::string module_name;
    std::set<std::string> dependencies;
};