#ifndef LR1PARSER_GRAMMAR_H
#define LR1PARSER_GRAMMAR_H


#include <string>
#include <unordered_set>
#include <utility>

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
  explicit Grammar(const Set<char>& terminals,
                   const Set<char>& nonterminals,
                   const Set<ProductionRule>& production_rules,
                   const char start): terminals_(terminals),
                                       nonterminals_(nonterminals),
                                       production_rules_(production_rules),
                                       start_(start) {};

  [[nodiscard]] char GetStartSymbol() const;
  [[nodiscard]] Set<char> GetNonTerminals() const;
  [[nodiscard]] Set<char> GetTerminals() const;
  [[nodiscard]] Set<ProductionRule> GetProductionRules() const;
 private:
  Set<char> terminals_ = {};
  Set<char> nonterminals_ = {};
  Set<ProductionRule> production_rules_ = {};
  char start_ = '\0';
};


#endif
