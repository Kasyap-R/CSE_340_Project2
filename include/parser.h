#include "lexer.h"
#include "types.h"
#include <unordered_set>
#include <vector>
class Parser {
  public:
    Parser();
    void parse_input();
    auto generate_execution_context() -> ExecutionContext;

  private:
    LexicalAnalyzer lexer;
    std::vector<Rule> rules;
    std::unordered_set<std::string> universe;
    std::vector<std::string> universe_order;
    std::unordered_set<std::string> non_terms;
    std::unordered_set<std::string> terms;
    std::vector<std::string> non_term_order;
    std::vector<std::string> term_order;

    void parse_grammar();
    void parse_rule_list();
    auto parse_rule() -> std::vector<Rule>;
    auto parse_rhs() -> std::vector<IDList>;
    auto parse_id_list() -> IDList;

    void update_universe(const Token &tok);

    static void syntax_error();
    auto expect(TokenType expected_type) -> Token;
};
