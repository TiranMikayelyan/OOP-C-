#include <iostream>
#include <string>

class Calculator {
private:
    std::string expr;
    int pos;

    double vars[26];   

    double parseExpression() {
        double value = parseTerm();

        while (pos < expr.size()) {
            if (expr[pos] == '+') {
                pos++;
                value += parseTerm();
            }
            else if (expr[pos] == '-') {
                pos++;
                value -= parseTerm();
            }
            else break;
        }

        return value;
    }

    double parseTerm() {
        double value = parseFactor();

        while (pos < expr.size()) {
            if (expr[pos] == '*') {
                pos++;
                value *= parseFactor();
            }
            else if (expr[pos] == '/') {
                pos++;
                value /= parseFactor();
            }
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
            pos++;

            return vars[idx]; // ուղղակի վերադարձնում ենք արժեքը
        }

        double num = 0;

        while (pos < expr.size() && expr[pos] >= '0' && expr[pos] <= '9') {
            num = num * 10 + (expr[pos] - '0');
            pos++;
        }

        return num;
    }

public:

    void setVar(char name, double value) {
        vars[name - 'a'] = value;   // փոփոխականին արժեք ենք տալիս
    }

    double calculate(std::string s) {

        expr = s;
        pos = 0;

        return parseExpression();
    }
};

int main() {

    Calculator calc;
    std::string expr;

    std::cout << "Enter expression: ";
    std::getline(std::cin, expr);

    for (int a = 0; a <= 100; a++) {

        calc.setVar('a', a); // a-ին տալիս ենք նոր արժեք

        double result = calc.calculate(expr);

        std::cout << "a = " << a << " -> Result = " << result << std::endl;
    }
}
