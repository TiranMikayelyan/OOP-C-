#include <iostream>
#include <string>
#include <memory>
#include <cctype>
#include <map>
#include <stack>
#include <variant>
#include <vector>

using Matrix = std::vector<std::vector<double>>;  // Matrix tipy
using Value = std::variant<double, Matrix>;       // Scalar kam matrix variant

// ===== Token =====
enum class TokenType { NUMBER, VAR, PLUS, MINUS, MUL, DIV, LPAREN, RPAREN, END };

// token struct
struct Token { TokenType type; std::string value; };

// ===== Lexer =====
class Lexer {

    std::string text;   // expression-i text
    size_t pos;         // index-inq
    char cur;           // hima vor char enq kardum

public:
    Lexer(const std::string& t) : text(t), pos(0), cur(t[0]) {}

    void advance() { pos++; cur = pos < text.size() ? text[pos] : '\0'; } // index-i amenangnel

    void skip() { while(cur && isspace(cur)) advance(); } // skip enq spaces

    Token getToken() {
        skip(); // skip spaces
        if (!cur) return {TokenType::END,""}; // ete texteri het e

        if (isdigit(cur)) { // tiv token
            std::string val;
            while(cur && isdigit(cur)) { val+=cur; advance(); }
            return {TokenType::NUMBER,val};
        }

        if (isalpha(cur)) { // variable token
            std::string val;
            while(cur && isalnum(cur)) { val+=cur; advance(); }
            return {TokenType::VAR,val};
        }

        char c = cur; advance(); // operator token

        switch(c){
            case '+': return {TokenType::PLUS,"+"};
            case '-': return {TokenType::MINUS,"-"};
            case '*': return {TokenType::MUL,"*"};
            case '/': return {TokenType::DIV,"/"};
            case '(': return {TokenType::LPAREN,"("};
            case ')': return {TokenType::RPAREN,")"};
            default: throw std::runtime_error("Invalid char"); // ete ancanot simvol
        }
    }
};

// ===== AST Node =====
enum class NodeType { NUMBER, VAR, BINARY };

struct ASTNode {
    NodeType type;                  // node-i tipy
    double num;                     // ete scalar
    std::string name;               // ete variable
    char op;                        // operator
    std::shared_ptr<ASTNode> left,right; // children

    ASTNode(double v): type(NodeType::NUMBER), num(v) {} // scalar constructor
    ASTNode(const std::string& n): type(NodeType::VAR), name(n) {} // variable constructor
    ASTNode(char o, std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r)
        : type(NodeType::BINARY), op(o), left(l), right(r) {} // operator constructor
};

// ===== Parser =====
class Parser {
    Lexer& lex;  // lexer object
    Token cur;   // hima vor token kardum

    void eat(TokenType t) { // check enq token chisht e
        if(cur.type==t) cur = lex.getToken();
        else throw std::runtime_error("Unexpected token");
    }

    std::shared_ptr<ASTNode> factor() {
        if(cur.type==TokenType::NUMBER){
            auto n = std::make_shared<ASTNode>(std::stod(cur.value));
            eat(TokenType::NUMBER);
            return n;
        }
        if(cur.type==TokenType::VAR){
            auto n = std::make_shared<ASTNode>(cur.value);
            eat(TokenType::VAR);
            return n;
        }
        if(cur.type==TokenType::LPAREN){
            eat(TokenType::LPAREN);
            auto n = expression();
            eat(TokenType::RPAREN);
            return n;
        }
        throw std::runtime_error("Invalid factor");
    }

    std::shared_ptr<ASTNode> term() {
        auto n = factor();
        while(cur.type==TokenType::MUL || cur.type==TokenType::DIV){
            char op = cur.value[0];
            eat(cur.type);
            n = std::make_shared<ASTNode>(op,n,factor());
        }
        return n;
    }

    std::shared_ptr<ASTNode> expression() {
        auto n = term();
        while(cur.type==TokenType::PLUS || cur.type==TokenType::MINUS){
            char op = cur.value[0];
            eat(cur.type);
            n = std::make_shared<ASTNode>(op,n,term());
        }
        return n;
    }

public:
    Parser(Lexer& l): lex(l), cur(l.getToken()) {}
    std::shared_ptr<ASTNode> parse() { return expression(); }
};

// ===== Print AST =====
void printAST(const std::shared_ptr<ASTNode>& root){
    std::stack<std::pair<std::shared_ptr<ASTNode>, int>> st;
    st.push({root,0});
    while(!st.empty()){
        auto [n,ind] = st.top(); st.pop();
        std::string sp(ind,' ');
        if(n->type==NodeType::NUMBER) std::cout<<sp<<"Number: "<<n->num<<"\n";
        else if(n->type==NodeType::VAR) std::cout<<sp<<"Var: "<<n->name<<"\n";
        else{
            std::cout<<sp<<"Op: "<<n->op<<"\n";
            if(n->right) st.push({n->right,ind+2});
            if(n->left) st.push({n->left,ind+2});
        }
    }
}

// ===== Eval AST stack-ov, scalar + matrix =====
Value evalAST_matrix(const std::shared_ptr<ASTNode>& root,
                     const std::map<std::string, Value>& vars){
    std::stack<std::shared_ptr<ASTNode>> st;
    std::map<ASTNode*, Value> values;
    st.push(root);

    auto applyOp = [](const Value& l, const Value& r, char op) -> Value{
        if(std::holds_alternative<double>(l) && std::holds_alternative<double>(r)){
            double lv=std::get<double>(l), rv=std::get<double>(r);
            switch(op){ case '+': return lv+rv; case '-': return lv-rv;
                         case '*': return lv*rv; case '/': return lv/rv; }
        }
        const Matrix* lm = std::holds_alternative<Matrix>(l) ? &std::get<Matrix>(l) : nullptr;
        const Matrix* rm = std::holds_alternative<Matrix>(r) ? &std::get<Matrix>(r) : nullptr;

        if(lm && rm){
            size_t rows=lm->size(), cols=lm->at(0).size();
            Matrix res(rows,std::vector<double>(cols));
            for(size_t i=0;i<rows;i++)
                for(size_t j=0;j<cols;j++)
                    switch(op){
                        case '+': res[i][j]=(*lm)[i][j]+(*rm)[i][j]; break;
                        case '-': res[i][j]=(*lm)[i][j]-(*rm)[i][j]; break;
                        case '*': res[i][j]=(*lm)[i][j]*(*rm)[i][j]; break;
                        case '/': res[i][j]=(*lm)[i][j]/(*rm)[i][j]; break;
                    }
            return res;
        }

        if(lm && !rm){
            double rv=std::get<double>(r);
            Matrix res=*lm;
            for(auto& row:res) for(auto& x:row) switch(op){
                case '+': x+=rv; break; case '-': x-=rv; break;
                case '*': x*=rv; break; case '/': x/=rv; break;
            }
            return res;
        }
        if(!lm && rm){
            double lv=std::get<double>(l);
            Matrix res=*rm;
            for(auto& row:res) for(auto& x:row) switch(op){
                case '+': x+=lv; break; case '-': x=lv-x; break;
                case '*': x*=lv; break; case '/': x=lv/x; break;
            }
            return res;
        }
        throw std::runtime_error("Invalid operation for matrix/scalar");
    };

    while(!st.empty()){
        auto n=st.top();
        if(n->type==NodeType::NUMBER){ values[n.get()]=n->num; st.pop(); }
        else if(n->type==NodeType::VAR){
            if(vars.count(n->name)==0) throw std::runtime_error("Variable not defined: "+n->name);
            values[n.get()]=vars.at(n->name);
            st.pop();
        }
        else{
            if(values.count(n->left.get())==0) st.push(n->left);
            else if(values.count(n->right.get())==0) st.push(n->right);
            else{ values[n.get()]=applyOp(values[n->left.get()], values[n->right.get()], n->op); st.pop(); }
        }
    }

    return values[root.get()];
}

// ===== Print Value =====
void printValue(const Value& val){
    if(std::holds_alternative<double>(val)) std::cout<<std::get<double>(val)<<"\n";
    else{
        const Matrix& m=std::get<Matrix>(val);
        for(const auto& row:m){
            for(double x:row) std::cout<<x<<" ";
            std::cout<<"\n";
        }
    }
}

// ===== Main =====
int main(){
    std::string expr;
    std::cout<<"Enter expression: ";
    std::getline(std::cin, expr);

    Lexer lexer(expr);
    Parser parser(lexer);

    try{
        auto tree = parser.parse();
        std::cout<<"\nAST Tree:\n";
        printAST(tree);

        std::map<std::string, Value> vars;
        std::string varName,line;
        std::cout<<"\nEnter variable values (type 'done' to finish):\n";

        while(true){
            std::cout<<"Variable name: ";
            std::cin>>varName;
            if(varName=="done") break;

            std::cout<<"Value (matrix format [[1,2],[3,4]]): ";
            std::cin.ignore(); std::getline(std::cin,line);

            if(line[0]=='['){ // simple matrix parser
                Matrix mat;
                size_t pos=0;
                while((pos=line.find('[',pos))!=std::string::npos){
                    size_t end=line.find(']',pos);
                    std::string rowstr=line.substr(pos+1,end-pos-1);
                    std::vector<double> row;
                    size_t p=0;
                    while(p<rowstr.size()){
                        size_t comma=rowstr.find(',',p);
                        std::string num=rowstr.substr(p,comma==std::string::npos? std::string::npos:comma-p);
                        row.push_back(std::stod(num));
                        if(comma==std::string::npos) break; else p=comma+1;
                    }
                    mat.push_back(row);
                    pos=end+1;
                }
                vars[varName]=mat;
            } else { // scalar
                vars[varName]=std::stod(line);
            }
        }

        Value result=evalAST_matrix(tree,vars);
        std::cout<<"\nResult:\n";
        printValue(result);

    }catch(std::exception& e){ std::cout<<"Error: "<<e.what()<<"\n"; }
}
