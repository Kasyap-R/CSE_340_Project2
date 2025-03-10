#pragma once
#include <string>
#include <unordered_set>
#include <vector>

using IDList = std::vector<std::string>;

struct Rule {
    std::string lhs;
    IDList rhs;
};

struct ExecutionContext {
    std::unordered_set<std::string> non_terms;
    std::unordered_set<std::string> terms;
    std::vector<std::string> non_term_order;
    std::vector<std::string> term_order;
    std::vector<Rule> rules;
};
