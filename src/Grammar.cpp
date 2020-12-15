#include "Grammar.h"

Set<char> Grammar::GetNonTerminals() const {
  return nonterminals_;
}

Set<char> Grammar::GetTerminals() const {
  return terminals_;
}

Set<ProductionRule> Grammar::GetProductionRules() const {
  return production_rules_;
}

char Grammar::GetStartSymbol() const {
  return start_;
}
