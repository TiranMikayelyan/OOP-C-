#pragma once
#include <vector>
#include <memory>
#include "tokenizer.h"
#include "ast.h"

class Parser {
    std::vector<Token> tokens;
    size_t pos = 0;
public:
    Parser(const std::vector<Token>& t) : tokens(t) {}
    std::shared_ptr<ASTNode> parseExpression();
private:
    std::shared_ptr<ASTNode> parseTerm();
    std::shared_ptr<ASTNode> parseFactor();
    Token peek() const;
    Token get();
};