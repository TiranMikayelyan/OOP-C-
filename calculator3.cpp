#include <iostream>   
#include <string>     
#include <memory>     // shared_ptr օգտագործելու համար
#include <cctype>     // isdigit, isalpha, isspace ֆունկցիաների համար
#include <map>        

// ===== Token =====
// տարբեր token-ների տեսակներն ենք սահմանում

enum class TokenType { NUMBER, VAR, PLUS, MINUS, MUL, DIV, LPAREN, RPAREN, END };

// token կառուցվածք
// type -> ինչ տեսակ է
// value -> իրական արժեքը (օրինակ "25" կամ "a")
struct Token { TokenType type; std::string value; };


// ===== Lexer =====
// Lexer-ի գործը տեքստը բաժանել token-ների

class Lexer {
    std::string text;   // ամբողջ արտահայտությունը
    size_t pos;         // որտեղ ենք հիմա տեքստի մեջ
    char cur;           // ներկայիս սիմվոլը

public:

    // constructor
    Lexer(const std::string& t) : text(t), pos(0), cur(t[0]) {}

    // անցնում ենք հաջորդ սիմվոլին
    void advance() {
        pos++;
        cur = pos < text.size() ? text[pos] : '\0';
    }

    // բացատները skip ենք անում
    void skip() {
        while (cur && isspace(cur))
            advance();
    }

    // ստանում ենք հաջորդ token-ը
    Token getToken() {

        skip(); // սկզբում բացատները բաց ենք թողնում

        if (!cur)
            return { TokenType::END,"" }; // եթե տեքստը վերջացել է

        // եթե թիվ է
        if (isdigit(cur)) {
            std::string val;

            // քանի դեռ թիվ է կարդում ենք
            while (cur && isdigit(cur)) {
                val += cur;
                advance();
            }

            return { TokenType::NUMBER,val };
        }

        // եթե փոփոխական է 
        if (isalpha(cur)) {
            std::string val;

            // կարդում ենք ամբողջ անունը
            while (cur && isalnum(cur)) {
                val += cur;
                advance();
            }

            return { TokenType::VAR,val };
        }

        // եթե օպերատոր է
        char c = cur;
        advance();

        switch (c) {
        case '+': return { TokenType::PLUS,"+" };
        case '-': return { TokenType::MINUS,"-" };
        case '*': return { TokenType::MUL,"*" };
        case '/': return { TokenType::DIV,"/" };
        case '(': return { TokenType::LPAREN,"(" };
        case ')': return { TokenType::RPAREN,")" };

        default:
            throw std::runtime_error("Invalid char"); // եթե սխալ սիմվոլ է
        }
    }
};



// ===== AST Node Abstract Syntax Tree =====
// այսինքն արտահայտության ծառ

enum class NodeType { NUMBER, VAR, BINARY };

// ծառի node
struct ASTNode {

    NodeType type;   // node-ի տեսակը

    double num;      // եթե թիվ է
    std::string name; // եթե փոփոխական է
    char op;         // եթե օպերատոր է

    // ձախ և աջ child-երը
    std::shared_ptr<ASTNode> left, right;

    // եթե թիվ է
    ASTNode(double v) : type(NodeType::NUMBER), num(v) {}

    // եթե փոփոխական է
    ASTNode(const std::string& n) : type(NodeType::VAR), name(n) {}

    // եթե օպերատոր է
    ASTNode(char o, std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r)
        : type(NodeType::BINARY), op(o), left(l), right(r) {}
};



// ===== Parser =====
// Parser-ը token-ներից սարքում է AST ծառ

class Parser {

    Lexer& lex; // lexer
    Token cur;  // ներկայիս token

    // ստուգում ենք token-ը ճիշտ է թե չէ
    void eat(TokenType t) {

        if (cur.type == t)
            cur = lex.getToken(); // վերցնում ենք հաջորդ token-ը
        else
            throw std::runtime_error("Unexpected token");
    }

    // factor -> թիվ | փոփոխական | (արտահայտություն)
    std::shared_ptr<ASTNode> factor() {

        // եթե թիվ է
        if (cur.type == TokenType::NUMBER) {

            auto n = std::make_shared<ASTNode>(std::stod(cur.value));

            eat(TokenType::NUMBER);

            return n;
        }

        // եթե փոփոխական է
        if (cur.type == TokenType::VAR) {

            auto n = std::make_shared<ASTNode>(cur.value);

            eat(TokenType::VAR);

            return n;
        }

        // եթե փակագիծ է
        if (cur.type == TokenType::LPAREN) {

            eat(TokenType::LPAREN);

            auto n = expression(); // ներսի արտահայտությունը

            eat(TokenType::RPAREN);

            return n;
        }

        throw std::runtime_error("Invalid factor");
    }


    // term -> factor (* կամ / factor)
    std::shared_ptr<ASTNode> term() {

        auto n = factor();

        while (cur.type == TokenType::MUL || cur.type == TokenType::DIV) {

            char op = cur.value[0];

            eat(cur.type);

            n = std::make_shared<ASTNode>(op, n, factor());
        }

        return n;
    }


    // expression -> term (+ կամ - term)
    std::shared_ptr<ASTNode> expression() {

        auto n = term();

        while (cur.type == TokenType::PLUS || cur.type == TokenType::MINUS) {

            char op = cur.value[0];

            eat(cur.type);

            n = std::make_shared<ASTNode>(op, n, term());
        }

        return n;
    }

public:

    Parser(Lexer& l) : lex(l), cur(l.getToken()) {}

    // parse-ը սկսում է ամբողջ parsing-ը
    std::shared_ptr<ASTNode> parse() {
        return expression();
    }
};



// ===== Print AST =====
// ծառը տպում ենք որ տեսնենք ինչ կառուցվածք ստացվեց

void printAST(const std::shared_ptr<ASTNode>& n, int ind = 0) {

    if (!n)
        return;

    std::string sp(ind, ' ');

    if (n->type == NodeType::NUMBER)
        std::cout << sp << "Number: " << n->num << "\n";

    else if (n->type == NodeType::VAR)
        std::cout << sp << "Var: " << n->name << "\n";

    else {

        std::cout << sp << "Op: " << n->op << "\n";

        printAST(n->left, ind + 2);

        printAST(n->right, ind + 2);
    }
}



// ===== Evaluate AST =====
// այստեղ հաշվարկում ենք ծառի արժեքը

double evalAST(const std::shared_ptr<ASTNode>& n,
               const std::map<std::string, double>& vars) {

    // եթե թիվ է
    if (n->type == NodeType::NUMBER)
        return n->num;

    // եթե փոփոխական է
    if (n->type == NodeType::VAR) {

        auto it = vars.find(n->name);

        if (it == vars.end())
            throw std::runtime_error("Variable not defined: " + n->name);

        return it->second;
    }

    // հաշվում ենք ձախ և աջ subtree-երը
    double l = evalAST(n->left, vars);
    double r = evalAST(n->right, vars);

    // օպերատորի հիման վրա հաշվում ենք
    switch (n->op) {

    case '+': return l + r;
    case '-': return l - r;
    case '*': return l * r;
    case '/': return l / r;

    default:
        throw std::runtime_error("Unknown op");
    }
}

// ===== Main =====

int main() {

    std::string expr;

    // օգտվողից վերցնում ենք արտահայտությունը
    std::cout << "Enter expression: ";
    std::getline(std::cin, expr);

    Lexer lexer(expr);

    Parser parser(lexer);

    try {

        // սարքում ենք AST ծառը
        auto tree = parser.parse();

        std::cout << "\nAST Tree:\n";

        printAST(tree);

        // փոփոխականների արժեքները այստեղ ենք պահելու
        std::map<std::string, double> vars;

        std::string varName;

        double varVal;

        std::cout << "\nEnter variable values (type 'done' to finish):\n";

        // օգտվողից փոփոխականների արժեքներն ենք վերցնում
        while (true) {

            std::cout << "Variable name: ";
            std::cin >> varName;

            if (varName == "done")
                break;

            std::cout << "Value: ";
            std::cin >> varVal;

            vars[varName] = varVal;
        }

        // վերջնական արդյունքը
        double result = evalAST(tree, vars);

        std::cout << "\nResult: " << result << "\n";

    }

    catch (std::exception& e) {

        // եթե սխալ լինի
        std::cout << "Error: " << e.what() << "\n";
    }
}
