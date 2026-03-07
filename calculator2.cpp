#include <iostream>
#include <string>

class Calculator {
private:
    std::string expr;   // Արտահայտությունը, որը հաշվելու ենք
    int pos;            // Հիմնական ինդեքսը՝ expr-ում գտնվելու դիրքը

    double vars[26];    // Փոփոխականների արժեքները a-z
    bool used[26];      // Որքան փոփոխականը արդեն հարցվել է օգտատիրոջից

    // Expression → Term { + Term | - Term }
    double parseExpression() {
        double value = parseTerm();  // Նախ՝ վերցնում ենք առաջին Term-ը

        while (pos < expr.size()) {  // Մինչեւ տողի վերջը
            if (expr[pos] == '+') {  // Եթե '+' 
                pos++;               // անցնենք հաջորդ սիմվոլին
                value += parseTerm(); // հաշվենք հաջորդ Term-ը և գումարել
            }
            else if (expr[pos] == '-') { // Եթե '-' հանդիպեց
                pos++;                  //  հաջորդ սիմվոլին
                value -= parseTerm();   // հաշվենք հաջորդ Term-ը և հանել
            }
            else break;  // Ոչ '+' և ոչ '-'  դուրս գանք loop-ից
        }

        return value;   // Վերադարձնել հաշվված Expression-ը
    }

    // Term → Factor { * Factor | / Factor }
    double parseTerm() {
        double value = parseFactor(); //  վերցնում ենք առաջին Factor-ը

        while (pos < expr.size()) {   // Մինչեւ տողի վերջը
            if (expr[pos] == '*') {  // Եթե '*' հանդիպեց
                pos++;               // անցնենք մինչև հաջորդ սիմվոլը
                value *= parseFactor(); // հաշվենք հաջորդ Factor-ը և բազմապատկենք
            }
            else if (expr[pos] == '/') { // Եթե '/' հանդիպեց
                pos++;                  // անցնեք հաջորդ սիմվոլին
                value /= parseFactor(); // հաշվենք հաջորդ Factor-ը և բաժանել
            }
            else break;  // Ոչ '*' և ոչ '/' դուրս գալ loop-ից
        }

        return value;   // Վերադարձնել հաշվված Term-ը
    }

    // Factor → Number | Variable | '(' Expression ')'
    double parseFactor() {

        if (expr[pos] == '(') {     // Եթե բաց փակագիծ
            pos++;                  // Գնալ '(' հետո
            double value = parseExpression(); // Հաշվել ներսի Expression-ը
            pos++;                  
            return value;           
        }

        if (expr[pos] >= 'a' && expr[pos] <= 'z') { // Եթե տառ-փոփոխական է
            int idx = expr[pos] - 'a';             // հաշվենք զանգվածի ինդեքսը

            if (!used[idx]) {                       // Եթե արժեք տված չի
                std::cout << "Enter value for " << expr[pos] << ": "; // 
                std::cin >> vars[idx];            // Պահել արժեքը
                used[idx] = true;                 // արդեն հարցրել ենք
            }

            pos++;                                 // Գնալ հաջորդ սիմվոլին
            return vars[idx];                      
        }

        double num = 0;                             // Եթե թիվ է, սկիզբը 0
        while (pos < expr.size() && expr[pos] >= '0' && expr[pos] <= '9') { // while-ը կարդում է ամբողջ թիվը
            num = num * 10 + (expr[pos] - '0');    // Հաշվել բազմանիշ թիվը
            pos++;                                 // Գնալ հաջորդ սիմվոլին
        }

        return num;                                // Վերադարձնել թիվը
    }

public:
    double calculate(std::string s) {
        expr = s;                                 // Պահել մուտքային տողը
        pos = 0;                                  // Սկսել սկզբից

        for (int i = 0; i < 26; i++) {           
            vars[i] = 0;                          // Փոփոխականների սկզբնական արժեքը 0
            used[i] = false;                      // Առայժմ ոչինչ չի հարցվել
        }

        return parseExpression();                 
    }
};

int main() {

    Calculator calc;                               
    std::string expr;                              

    std::cout << "Enter expression: ";            
    std::getline(std::cin, expr);                 

    std::cout << "Result: " << calc.calculate(expr) << std::endl; //
}