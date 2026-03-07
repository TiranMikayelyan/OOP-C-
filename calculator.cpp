#include <iostream>
#include <string>


class Calculator {
private:
    double numbers[1000]; // թվերի համար զանգված
    int numTop;           // վերջին ավելացված տարրի ինդեքսը

    char ops[1000];    // արտահայտության համար
    int opTop;         // վերջին դրված օպերատորի տեղը


    int priority(char op) {
        if (op == '+' || op == '-') return 1; //2-րդային մակարդակի գործողություն
        if (op == '*' || op == '/') return 2;   // 1-նային մակարդակի գործողություն
        return 0; //մնացածը 0

    }

    double Operation(double a, double b, char op) { // իրական գործողությունը կատարող ֆունկցիա
        if (op == '+') return a + b;             // գումարում
        if (op == '-') return a - b;             // հանում
        if (op == '*') return a * b;             // բազմապատկում
        if (op == '/') return a / b;             // բաժանում
        return 0;                               // եթե ուրիշ բան լինի
    }

    void processHighLevelOperation() {                // stack-ի վերևի գործողությունը հաշվելու ֆունկցիա
        double right = numbers[numTop--];       // վերցնում ենք աջ թիվը ու numTop-ը նվազեցնում
        double left = numbers[numTop--];        // վերցնում ենք ձախ թիվը ու numTop-ը նվազեցնում
        char op = ops[opTop--];                 // վերցնում ենք operator-ը ու opTop-ը նվազեցնում

        double result = Operation(left, right, op); // հաշվում ենք left op right
        numbers[++numTop] = result;             // արդյունքը նորից push ենք անում numbers stack
    }

public:
    Calculator() {                              // կոնստրուկտոր
        numTop = -1;                            // stack-ը դատարկ ա
        opTop = -1;                             // stack-ը դատարկ ա
    }

    double calculate(const std::string& expr) {      // հիմնական ֆունկցիա, որը հաշվում ա արտահայտությունը
        for (int i = 0; i < expr.size(); i++) {  // անցնում ենք ամբողջ տողի վրայով

            if (expr[i] == ' ') continue;        // եթե space ա՝ բաց ենք թողնում

            if (isdigit(expr[i])) {              // եթե թիվ ա (0-9)
                double num = 0;                 // թիվը հավաքելու համար փոփոխական

                while (i < expr.size() && isdigit(expr[i])) { // կարդում ենք ամբողջ թիվը (օր. 123)
                    num = num * 10 + (expr[i] - '0');        // թիվը սարքում ենք
                    i++;                         // գնում ենք հաջորդ սիմվոլին
                }

                i--;                             // քանի որ while-ից դուրս եկանք, մի քայլ հետ ենք գալիս
                numbers[++numTop] = num;         // թիվը push ենք անում numbers stack
            }

            else if (expr[i] == '(') {            // եթե բաց փակագիծ ա
                ops[++opTop] = '(';              // push ենք անում ops stack
            }

            else if (expr[i] == ')') {            // եթե փակ փակագիծ ա
                while (opTop >= 0 && ops[opTop] != '(') { // հաշվում ենք մինչև գտնենք '('
                    processHighLevelOperation();       // կատարում ենք գործողություն
                }

                if (opTop >= 0 && ops[opTop] == '(') { // եթե գտանք '('
                    opTop--;                     // pop ենք անում '('
                }
            }

            else if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') {
                // եթե գործողություն ա (+ - * /)

                while (opTop >= 0 && ops[opTop] != '(' &&
                    priority(ops[opTop]) >= priority(expr[i])) {
                    // եթե վերևի operator-ը ավելի ուժեղ կամ հավասար է նոր operator-ին
                    processHighLevelOperation();       // նախ հաշվում ենք stack-ի վերևի գործողությունը
                }

                ops[++opTop] = expr[i];          // հետո նոր operator-ը push ենք անում ops stack
            }
        }

        while (opTop >= 0) {                      // եթե operator stack-ում դեռ բան կա
            processHighLevelOperation();               // բոլորը հերթով հաշվում ենք
        }

        return numbers[numTop];                  // վերջում numbers stack-ի վերևը արդյունքն ա
    }
};

int main() {
    Calculator calc;                             // Calculator օբյեկտ ենք ստեղծում
    std::string expr;                                 // տող, որտեղ պահելու ենք արտահայտությունը

    std::cout << "Enter expression: ";                // տպում ենք հարցը
    std::getline(std::cin, expr);                          // կարդում ենք ամբողջ տողը

    std::cout << "Result: " << calc.calculate(expr) << std::endl; // հաշվում ենք ու տպում արդյունքը

    return 0;                                    // ծրագիրը ավարտվում է
}
