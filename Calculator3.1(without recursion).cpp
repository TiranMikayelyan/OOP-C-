#include <iostream>
#include <string>
#include <memory>
#include <cctype>
#include <map>
#include <stack>

// ===== Token =====
// stegh stexcum enq token-i tesakner@ (inch tipi simvol e)

enum class TokenType { NUMBER, VAR, PLUS, MINUS, MUL, DIV, LPAREN, RPAREN, END };

// token struktura
// type -> token-i tesak@
// value -> irakan arjeq@ (orinak "25" kam "a")
struct Token { TokenType type; std::string value; };


// ===== Lexer =====
// Lexer-i gorc@ tekst@ bajanel token-neri

class Lexer {

    std::string text;   // amboxj expression@ pahum e
    size_t pos;         // vor index-um gtnvum enq
    char cur;           // hima vor simvoln enq kardum

public:

    // constructor
    Lexer(const std::string& t) : text(t), pos(0), cur(t[0]) {}

    // ancnum enq hajord simvolin
    void advance() {
        pos++;
        cur = pos < text.size() ? text[pos] : '\0';
    }

    // bacatner@ skip enq anum
    void skip() {
        while (cur && isspace(cur))
            advance();
    }

    // vercnum enq hajord token@
    Token getToken() {

        skip(); // skzbic bacatner@ ancnum enq

        if (!cur)
            return { TokenType::END,"" }; // ete tekst@ avartvec

        // ete tiv e
        if (isdigit(cur)) {

            std::string val;

            // qani der tiv e kardum enq
            while (cur && isdigit(cur)) {
                val += cur;
                advance();
            }

            return { TokenType::NUMBER,val };
        }

        // ete popoxakan e
        if (isalpha(cur)) {

            std::string val;

            // kardum enq amboxj anun@
            while (cur && isalnum(cur)) {
                val += cur;
                advance();
            }

            return { TokenType::VAR,val };
        }

        // ete operator e
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
            throw std::runtime_error("Invalid char"); // ete ancanot simvol e
        }
    }
};



// ===== AST Node =====
// sa expression-i tsarrn e (Abstract Syntax Tree)

enum class NodeType { NUMBER, VAR, BINARY };

struct ASTNode {

    NodeType type;   // node-i tesak@

    double num;      // ete tiv e
    std::string name; // ete popoxakan e
    char op;         // ete operator e

    // dzax u aj child-eri ham
    std::shared_ptr<ASTNode> left, right;

    // ete tiv e
    ASTNode(double v) : type(NodeType::NUMBER), num(v) {}

    // ete popoxakan e
    ASTNode(const std::string& n) : type(NodeType::VAR), name(n) {}

    // ete operator e
    ASTNode(char o, std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r)
        : type(NodeType::BINARY), op(o), left(l), right(r) {}
};



// ===== Parser =====
// Parser@ token-neric sarqum e AST tsarr

class Parser {

    Lexer& lex; // lexer
    Token cur;  // hima vor tokenn enq kardum

    // stugum enq token@ chisht e te che
    void eat(TokenType t) {

        if (cur.type == t)
            cur = lex.getToken(); // vercnum enq hajord token@
        else
            throw std::runtime_error("Unexpected token");
    }

    // factor -> tiv | popoxakan | (expression)
    std::shared_ptr<ASTNode> factor() {

        if (cur.type == TokenType::NUMBER) {

            auto n = std::make_shared<ASTNode>(std::stod(cur.value));

            eat(TokenType::NUMBER);

            return n;
        }

        if (cur.type == TokenType::VAR) {

            auto n = std::make_shared<ASTNode>(cur.value);

            eat(TokenType::VAR);

            return n;
        }

        if (cur.type == TokenType::LPAREN) {

            eat(TokenType::LPAREN);

            auto n = expression(); // pakagci meji expression@

            eat(TokenType::RPAREN);

            return n;
        }

        throw std::runtime_error("Invalid factor");
    }

    // term -> factor (* kam / factor)
    std::shared_ptr<ASTNode> term() {

        auto n = factor();

        while (cur.type == TokenType::MUL || cur.type == TokenType::DIV) {

            char op = cur.value[0];

            eat(cur.type);

            n = std::make_shared<ASTNode>(op, n, factor());
        }

        return n;
    }

    // expression -> term (+ kam - term)
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

    // sksum e parse@ (sarqum e amboxj tsarr@)
    std::shared_ptr<ASTNode> parse() {
        return expression();
    }
};



// ===== Print AST (aranc recursion) =====
// stack-ov ancnum enq tsarr@ u tpum

void printAST(const std::shared_ptr<ASTNode>& root) {

    std::stack<std::pair<std::shared_ptr<ASTNode>, int>> st;

    st.push({ root,0 });

    while (!st.empty()) {

        auto [n, ind] = st.top();
        st.pop();

        std::string sp(ind, ' ');

        if (n->type == NodeType::NUMBER)
            std::cout << sp << "Number: " << n->num << "\n";

        else if (n->type == NodeType::VAR)
            std::cout << sp << "Var: " << n->name << "\n";

        else {

            std::cout << sp << "Op: " << n->op << "\n";

            // skzbic push enq aj@ vor dzax@ arajin tpvi
            if (n->right)
                st.push({ n->right, ind + 2 });

            if (n->left)
                st.push({ n->left, ind + 2 });
        }
    }
}



// ===== Evaluate AST (aranc recursion) =====
// postorder traversal stack-ov

double evalAST(const std::shared_ptr<ASTNode>& root,
               const std::map<std::string, double>& vars) {

    std::stack<std::shared_ptr<ASTNode>> st;

    std::map<ASTNode*, double> values;

    st.push(root);

    while (!st.empty()) {

        auto n = st.top();

        if (n->type == NodeType::NUMBER) {

            values[n.get()] = n->num;
            st.pop();
        }

        else if (n->type == NodeType::VAR) {

            values[n.get()] = vars.at(n->name);
            st.pop();
        }

        else {

            if (values.count(n->left.get()) == 0) {
                st.push(n->left);
            }
            else if (values.count(n->right.get()) == 0) {
                st.push(n->right);
            }
            else {

                double l = values[n->left.get()];
                double r = values[n->right.get()];
                double res;

                switch (n->op) {

                case '+': res = l + r; break;
                case '-': res = l - r; break;
                case '*': res = l * r; break;
                case '/': res = l / r; break;
                }

                values[n.get()] = res;

                st.pop();
            }
        }
    }

    return values[root.get()];
}



// ===== Main =====

int main() {

    std::string expr;

    // ogtagorcox@ mutq e talis expression
    std::cout << "Enter expression: ";
    std::getline(std::cin, expr);

    Lexer lexer(expr);

    Parser parser(lexer);

    try {

        // stexcum enq AST tsarr@
        auto tree = parser.parse();

        std::cout << "\nAST Tree:\n";

        printAST(tree);

        // stegh pahum enq popoxakanneri arjeqner@
        std::map<std::string, double> vars;

        std::string varName;

        double varVal;

        std::cout << "\nEnter variable values (type 'done' to finish):\n";

        // ogtagorcox@ mutq e talis popoxakanner@
        while (true) {

            std::cout << "Variable name: ";
            std::cin >> varName;

            if (varName == "done")
                break;

            std::cout << "Value: ";
            std::cin >> varVal;

            vars[varName] = varVal;
        }

        // hashvum e expression-i verjnakan arjeq@
        double result = evalAST(tree, vars);

        std::cout << "\nResult: " << result << "\n";

    }

    catch (std::exception& e) {

        // ete sxal lini
        std::cout << "Error: " << e.what() << "\n";
    }
}
