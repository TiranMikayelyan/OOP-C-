#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

class Calculator {
private:
    std::string expr;   
    int pos;            

    double vars[26];    
    bool used[26];      

    double parseExpression() {
        double value = parseTerm();
        while (pos < expr.size()) {
            if (expr[pos] == '+') { pos++; value += parseTerm(); }
            else if (expr[pos] == '-') { pos++; value -= parseTerm(); }
            else break;
        }
        return value;
    }

    double parseTerm() {
        double value = parseFactor();
        while (pos < expr.size()) {
            if (expr[pos] == '*') { pos++; value *= parseFactor(); }
            else if (expr[pos] == '/') { pos++; value /= parseFactor(); }
            else break;
        }
        return value;
    }

    double parseFactor() {
        if (expr[pos] == '(') {
            pos++;
            double value = parseExpression();
            pos++;
            return value;
        }

        if (expr[pos] >= 'a' && expr[pos] <= 'z') {
            int idx = expr[pos] - 'a';

            if (!used[idx]) {
                // Delta var = 1, range [0,100]
                vars[idx] = rand() % 101; // 0…100
                used[idx] = true;
                std::cout << "Variable " << expr[pos] << " assigned random value: " << vars[idx] << "\n";
            }

            pos++;
            return vars[idx];
        }

        double num = 0;
        while (pos < expr.size() && expr[pos] >= '0' && expr[pos] <= '9') {
            num = num * 10 + (expr[pos] - '0');
            pos++;
        }
        return num;
    }

public:
    Calculator() { srand(time(0)); } 

    double calculate(std::string s) {
        expr = s;
        pos = 0;
        for (int i = 0; i < 26; i++) { vars[i]=0; used[i]=false; }
        return parseExpression();
    }
};

int main() {
    Calculator calc;
    std::string expr;

    std::cout << "Enter expression: ";
    std::getline(std::cin, expr);

    std::cout << "Result: " << calc.calculate(expr) << std::endl;
}
