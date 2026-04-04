#pragma once
#include <string>
#include <vector>

enum class TokenType { NUMBER, VARIABLE, OPERATOR, LPAREN, RPAREN };

struct Token {
    TokenType type;
    std::string value;
};

class Tokenizer {
    std::string text;
    size_t pos = 0;
public:
    Tokenizer(const std::string& t) : text(t) {}
    std::vector<Token> tokenize();
};