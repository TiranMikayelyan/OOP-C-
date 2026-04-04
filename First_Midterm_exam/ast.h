#pragma once
#include <memory>
#include <variant>
#include <string>
#include "registry.h"

struct ASTNode {
    virtual ~ASTNode() = default;
};

struct NumberNode : ASTNode {
    double value;
    NumberNode(double v) : value(v) {}
};

struct VariableNode : ASTNode {
    std::string name;
    VariableNode(const std::string& n) : name(n) {}
};

struct UnaryNode : ASTNode {
    char op;
    std::shared_ptr<ASTNode> child;
    UnaryNode(char o, std::shared_ptr<ASTNode> c) : op(o), child(c) {}
};

struct BinaryNode : ASTNode {
    char op;
    std::shared_ptr<ASTNode> left, right;
    BinaryNode(std::shared_ptr<ASTNode> l, char o, std::shared_ptr<ASTNode> r)
        : left(l), op(o), right(r) {}
};