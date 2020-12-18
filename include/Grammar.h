#ifndef LR1PARSER_GRAMMAR_H
#define LR1PARSER_GRAMMAR_H


#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

template <typename T>
using Set = std::unordered_set<T>;

using ProductionRule = std::pair<char, std::string>;

namespace std {
  template <>
  struct hash<ProductionRule> {
    size_t operator()(const ProductionRule& key) const {
      size_t res = 17;
      res = res*31 + hash<char>()(key.first);
      res = res*31 + hash<string>()(key.second);
      return res;
    }
  };
}

class Grammar {
 public:
  explicit Grammar() = default;
  explicit Grammar(Set<char> terminals,
                   Set<char> nonterminals,
                   std::vector<ProductionRule> production_rules,
                   const char start):
                     terminals_(std::move(terminals)),
                     nonterminals_(std::move(nonterminals)),
                     production_rules_(std::move(production_rules)),
                     start_(start) {};

  [[nodiscard]] char GetStartSymbol() const;
  [[nodiscard]] Set<char> GetNonTerminals() const;
  [[nodiscard]] Set<char> GetTerminals() const;
  [[nodiscard]] std::vector<ProductionRule> GetProductionRules() const;
 private:
  Set<char> terminals_ = {};
  Set<char> nonterminals_ = {};
  std::vector<ProductionRule> production_rules_ = {};
  char start_ = '\0';
};


#endif
