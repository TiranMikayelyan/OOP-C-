#include <iostream>
#include "tokenizer.h"
#include "parser.h"
#include "registry.h"
#include "evaluator.h"

int main() {
    std::string line;
    Registry registry;

    std::cout << "Enter expression: ";
    std::getline(std::cin, line);

    Tokenizer tokenizer(line);
    auto tokens = tokenizer.tokenize();

    // Collect variables
    for (auto& t : tokens) {
        if (t.type == TokenType::VARIABLE && !registry.exists(t.value)) {
            double val;
            std::cout << "Enter value for " << t.value << ": ";
            std::cin >> val;
            registry.set(t.value, val);
        }
    }

    Parser parser(tokens);
    auto ast = parser.parseExpression();
    double result = evaluateMatrix(ast, registry);

    std::cout << "Result: " << result << "\n";
    return 0;
}
