#include "types.h"
#include <unordered_set>

using FirstMap =
    std::unordered_map<std::string, std::unordered_set<std::string>>;
namespace analysis {
auto print_nullable(const Grammar &grammar) -> void;
auto calc_nullable(const Grammar &grammar) -> std::unordered_set<std::string>;
auto print_first(const Grammar &grammar) -> void;
auto calc_first(const Grammar &grammar) -> FirstMap;
auto all_nullable(const std::vector<std::string> &symbols,
                  const std::unordered_set<std::string> &nullable) -> bool;

} // namespace analysis
