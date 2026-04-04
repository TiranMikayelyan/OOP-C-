#pragma once
#include <unordered_map>
#include <string>
#include <variant>
#include <stdexcept>

using Value = std::variant<double, std::string>;

class Registry {
    std::unordered_map<std::string, double> table;
public:
    void set(const std::string& name, double val) { table[name] = val; }
    double get(const std::string& name) const {
        auto it = table.find(name);
        if (it == table.end())
            throw std::runtime_error("Unknown variable: " + name);
        return it->second;
    }
    bool exists(const std::string& name) const { return table.count(name) > 0; }
};