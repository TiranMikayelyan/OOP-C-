#include "evaluator.h"
#include <stack>
#include <unordered_map>
#include <stdexcept>
#include <variant>

double resolveValue(const Value& v, Registry& reg) {
    if (std::holds_alternative<double>(v)) return std::get<double>(v);
    return reg.get(std::get<std::string>(v));
}

double evaluateMatrix(std::shared_ptr<ASTNode> root, Registry& registry) {
    struct Frame { std::shared_ptr<ASTNode> node; bool visited = false; };
    std::stack<Frame> st;
    std::unordered_map<ASTNode*, double> results;

    st.push({ root });

    while (!st.empty()) {
        auto& f = st.top();
        if (!f.visited) {
            f.visited = true;
            if (auto b = dynamic_cast<BinaryNode*>(f.node.get())) {
                st.push({ b->right,false });
                st.push({ b->left,false });
            }
            else if (auto u = dynamic_cast<UnaryNode*>(f.node.get())) {
                st.push({ u->child,false });
            }
        }
        else {
            if (auto n = dynamic_cast<NumberNode*>(f.node.get())) results[f.node.get()] = n->value;
            else if (auto v = dynamic_cast<VariableNode*>(f.node.get())) results[f.node.get()] = registry.get(v->name);
            else if (auto u = dynamic_cast<UnaryNode*>(f.node.get())) results[f.node.get()] = -results[u->child.get()];
            else if (auto b = dynamic_cast<BinaryNode*>(f.node.get())) {
                double l = results[b->left.get()];
                double r = results[b->right.get()];
                switch (b->op) {
                case '+': results[f.node.get()] = l + r; break;
                case '-': results[f.node.get()] = l - r; break;
                case '*': results[f.node.get()] = l * r; break;
                case '/': if (r == 0) throw std::runtime_error("Division by zero"); results[f.node.get()] = l / r; break;
                default: throw std::runtime_error("Unknown operator");
                }
            }
            st.pop();
        }
    }
    return results[root.get()];
}