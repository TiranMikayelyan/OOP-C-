#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <cctype>
#include <stdexcept>
#include <set>

// Value can be double or a variable name (string)
using Value = std::variant<double, std::string>;
std::unordered_map<std::string, double> variables; // Symbol table

// AST nodes
struct ASTNode {
    virtual ~ASTNode() = default;
    virtual Value eval() const = 0;
};

struct NumberNode : ASTNode {
    double value;
    NumberNode(double v) : value(v) {}
    Value eval() const override { return value; }
};

struct VariableNode : ASTNode {
    std::string name;
    VariableNode(const std::string& n) : name(n) {}
    Value eval() const override {
        if(variables.find(name) == variables.end())
            throw std::runtime_error("Unknown variable: " + name);
        return variables[name];
    }
};

struct UnaryNode : ASTNode {
    char op; // only '-'
    std::shared_ptr<ASTNode> child;
    UnaryNode(char o, std::shared_ptr<ASTNode> c) : op(o), child(c) {}
    Value eval() const override {
        auto val = child->eval();
        if(std::holds_alternative<double>(val)){
            if(op == '-') return -std::get<double>(val);
            return val;
        } else {
            throw std::runtime_error("Unary operation on variable not allowed directly");
        }
    }
};

struct BinaryNode : ASTNode {
    char op; // '+', '-', '*', '/'
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;
    BinaryNode(std::shared_ptr<ASTNode> l, char o, std::shared_ptr<ASTNode> r)
        : left(l), op(o), right(r) {}
    Value eval() const override {
        auto lv = left->eval();
        auto rv = right->eval();
        if(!std::holds_alternative<double>(lv) || !std::holds_alternative<double>(rv))
            throw std::runtime_error("Binary operation requires numbers");
        double l = std::get<double>(lv);
        double r = std::get<double>(rv);
        switch(op){
            case '+': return l + r;
            case '-': return l - r;
            case '*': return l * r;
            case '/':
                if(r == 0) throw std::runtime_error("Division by zero");
                return l / r;
            default: throw std::runtime_error("Unknown operator");
        }
    }
};

// Parser
class Parser {
    std::string text;
    size_t pos = 0;
public:
    Parser(const std::string& t) : text(t) {}
    
    char peek() const { return pos < text.size() ? text[pos] : '\0'; }
    char get() { return pos < text.size() ? text[pos++] : '\0'; }
    void skipWhitespace() { while(std::isspace(peek())) get(); }

    std::shared_ptr<ASTNode> parseExpression() {
        auto node = parseTerm();
        while(true){
            skipWhitespace();
            char op = peek();
            if(op != '+' && op != '-') break;
            get();
            node = std::make_shared<BinaryNode>(node, op, parseTerm());
        }
        return node;
    }

    std::shared_ptr<ASTNode> parseTerm() {
        auto node = parseFactor();
        while(true){
            skipWhitespace();
            char op = peek();
            if(op != '*' && op != '/') break;
            get();
            node = std::make_shared<BinaryNode>(node, op, parseFactor());
        }
        return node;
    }

    std::shared_ptr<ASTNode> parseFactor() {
        skipWhitespace();
        char c = peek();
        if(c == '-') {
            get();
            return std::make_shared<UnaryNode>('-', parseFactor());
        }
        if(c == '('){
            get();
            auto node = parseExpression();
            skipWhitespace();
            if(get() != ')') throw std::runtime_error("Expected ')'");
            return node;
        }
        if(std::isdigit(c) || c == '.'){
            std::string num;
            while(std::isdigit(peek()) || peek() == '.') num += get();
            return std::make_shared<NumberNode>(std::stod(num));
        }
        if(std::isalpha(c)){
            std::string var;
            while(std::isalnum(peek()) || peek() == '_') var += get();
            return std::make_shared<VariableNode>(var);
        }
        throw std::runtime_error("Invalid factor");
    }
};

// Extract variable names
std::set<std::string> extractVariables(const std::string& expr) {
    std::set<std::string> vars;
    size_t i = 0;
    while(i < expr.size()){
        if(std::isalpha(expr[i])){
            std::string var;
            while(i < expr.size() && (std::isalnum(expr[i]) || expr[i]=='_')) var += expr[i++];
            vars.insert(var);
        } else ++i;
    }
    return vars;
}

int main() {
    std::string line;
    std::cout << "Enter expression: ";
    std::getline(std::cin, line);

    auto vars = extractVariables(line);
    for(const auto& v : vars){
        if(variables.find(v) == variables.end()){
            std::cout << "Enter value for " << v << ": ";
            double val;
            std::cin >> val;
            variables[v] = val;
        }
    }

    try {
        Parser parser(line);
        auto ast = parser.parseExpression();
        auto result = ast->eval();
        if(std::holds_alternative<double>(result))
            std::cout << "Result: " << std::get<double>(result) << std::endl;
        else
            std::cout << "Result is variable: " << std::get<std::string>(result) << std::endl;
    } catch(std::exception& e){
        std::cout << "Error: " << e.what() << std::endl;
    }
    return 0;
}
