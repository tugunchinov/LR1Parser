#ifndef LR1PARSER_H
#define LR1PARSER_H


#include <unordered_map>
#include <variant>

#include "Grammar.h"
#include "gtest/gtest.h"

struct Situation {
  ProductionRule production_rule;
  int next_symbol_index;
  char expected_symbol;
  bool operator==(const Situation&) const = default;
};

namespace std {
  template <>
  struct hash<Situation> {
    size_t operator()(const Situation& key) const {
      size_t res = 17;
      res = res*31 + hash<ProductionRule>()(key.production_rule);
      res = res*31 + hash<int>()(key.next_symbol_index);
      res = res*31 + hash<char>()(key.expected_symbol);
      return res;
    }
  };

  template <>
  struct hash<std::pair<int, char>> {
    size_t operator()(const std::pair<int, char>& key) const {
      size_t res = 17;
      res = res*31 + hash<int>()(key.first);
      res = res*31 + hash<char>()(key.second);
      return res;
    }
  };
}

using State = Set<Situation>;

struct Accepted {};
enum {
  INT,
  PRODUCTION_RULE,
  ACCEPTED
};
using Action = std::variant<int, ProductionRule, Accepted>;

class LR1Parser {
 public:
  void Fit(const Grammar& grammar);
  [[nodiscard]] bool Predict(const std::string& word) const;
 private:
  Set<char> First_(const std::string& expression) const;
  Set<Situation> Closure_(const Set<Situation>& situations) const;
  Set<Situation> Goto_(const Set<Situation>& situations,
                       const char symbol) const;
  void MakeStates_(const Grammar& grammar);
  Situation Init_(const Grammar& grammar);
  void Clear_();

  std::unordered_map<std::pair<int, char>, Action> actions_;
  std::vector<State> states_;
  Set<char> nonterminals_;
  Set<char> terminals_;
  Set<ProductionRule> production_rules_;
  const char new_start_ = '$';
};


#endif
