#include "parser.h"
#include <stdexcept>

Token Parser::peek() const { return pos < tokens.size() ? tokens[pos] : Token{ TokenType::OPERATOR,"" }; }
Token Parser::get() { return pos < tokens.size() ? tokens[pos++] : Token{ TokenType::OPERATOR,"" }; }

std::shared_ptr<ASTNode> Parser::parseExpression() {
    auto node = parseTerm();
    while (true) {
        auto t = peek();
        if (t.type != TokenType::OPERATOR || (t.value != "+" && t.value != "-")) break;
        get();
        node = std::make_shared<BinaryNode>(node, t.value[0], parseTerm());
    }
    return node;
}

std::shared_ptr<ASTNode> Parser::parseTerm() {
    auto node = parseFactor();
    while (true) {
        auto t = peek();
        if (t.type != TokenType::OPERATOR || (t.value != "*" && t.value != "/")) break;
        get();
        node = std::make_shared<BinaryNode>(node, t.value[0], parseFactor());
    }
    return node;
}

std::shared_ptr<ASTNode> Parser::parseFactor() {
    auto t = get();
    if (t.type == TokenType::NUMBER)
        return std::make_shared<NumberNode>(std::stod(t.value));
    if (t.type == TokenType::VARIABLE)
        return std::make_shared<VariableNode>(t.value);
    if (t.type == TokenType::OPERATOR && t.value == "-")
        return std::make_shared<UnaryNode>('-', parseFactor());
    if (t.type == TokenType::LPAREN) {
        auto node = parseExpression();
        auto next = get();
        if (next.type != TokenType::RPAREN) throw std::runtime_error("Expected ')'");
        return node;
    }
    throw std::runtime_error("Invalid factor");
}