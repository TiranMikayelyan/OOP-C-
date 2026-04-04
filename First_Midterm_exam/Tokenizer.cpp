#include "tokenizer.h"
#include <cctype>
#include <stdexcept>

std::vector<Token> Tokenizer::tokenize() {
    std::vector<Token> tokens;
    while (pos < text.size()) {
        char c = text[pos];
        if (std::isspace(c)) { pos++; continue; }

        if (std::isdigit(c) || c == '.') {
            std::string num;
            while (pos < text.size() && (std::isdigit(text[pos]) || text[pos] == '.'))
                num += text[pos++];
            tokens.push_back({ TokenType::NUMBER, num });
        }
        else if (std::isalpha(c)) {
            std::string var;
            while (pos < text.size() && (std::isalnum(text[pos]) || text[pos] == '_'))
                var += text[pos++];
            tokens.push_back({ TokenType::VARIABLE, var });
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/') {
            tokens.push_back({ TokenType::OPERATOR, std::string(1,c) });
            pos++;
        }
        else if (c == '(') { tokens.push_back({ TokenType::LPAREN,"(" }); pos++; }
        else if (c == ')') { tokens.push_back({ TokenType::RPAREN,")" }); pos++; }
        else throw std::runtime_error("Unknown character");
    }
    return tokens;
}