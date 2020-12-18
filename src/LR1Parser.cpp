#include <stack>
#include <stdexcept>

#include "LR1Parser.h"

void LR1Parser::Fit(const Grammar& grammar) {
  Clear_();
  Situation end_situation = Init_(grammar);
  MakeStates_(grammar);
  for (int i = 0; i < states_.size(); ++i) {
    if (states_[i].contains(end_situation)) {
      actions_[{i, '\0'}] = Accept{};
      continue;
    }
    for (const auto& situation : states_[i]) {
      if (situation.next_symbol_index ==
          situation.production_rule.second.size()) {
        if (actions_.contains({i, situation.expected_symbol})) {
          throw std::invalid_argument("Ambiguous action.");
        } else {
          actions_[{i, situation.expected_symbol}] = situation.production_rule;
        }
      }
    }
  }
}

Situation LR1Parser::Init_(const Grammar& grammar) {
  ProductionRule start_rule{new_start_,
                            std::string(1, grammar.GetStartSymbol())};
  production_rules_ = grammar.GetProductionRules();
  nonterminals_ = grammar.GetNonTerminals();
  terminals_ = grammar.GetTerminals();
  Situation start_situation{start_rule, 0, '\0'};
  Situation end_situation{start_rule, 1, '\0'};
  State start_state = Closure_({start_situation});
  states_.push_back(start_state);
  return end_situation;
}

bool LR1Parser::Predict(const std::string& word) const {
  std::stack<std::pair<char, int>> stack;
  stack.push({'\0', 0});
  int pos = 0;
  char current_symbol = word[pos];
  while (true) {
    int state = stack.top().second;
    if (!actions_.contains({state, current_symbol})) {
      return false;
    }
    switch (actions_.at({state, current_symbol}).index()) {
      case SHIFT: {
        stack.push({current_symbol,
                    std::get<int>(actions_.at({state, current_symbol}))});
        current_symbol = word[++pos];
        break;
      }
      case REDUCE: {
        auto production_rule =
            std::get<ProductionRule>(actions_.at({state, current_symbol}));
        for (int i = 0; i < production_rule.second.size(); ++i) {
          stack.pop();
        }
        --pos;
        current_symbol = production_rule.first;
        break;
      }
      case ACCEPT: {
        return true;
      }
    }
  }
}

void LR1Parser::MakeStates_(const Grammar& grammar) {
  auto symbols = nonterminals_;
  symbols.insert(terminals_.begin(), terminals_.end());
  size_t old_size = 0;
  do {
    old_size = states_.size();
    for (int i = 0; i < states_.size(); ++i) {
      for (char symbol : symbols) {
        auto new_state = Goto_(states_[i], symbol);
        if (!new_state.empty()) {
          int index = std::find(states_.begin(),
                                states_.end(),
                                new_state) - states_.begin();
          if (index == states_.size()) {
            states_.push_back(new_state);
          }
          actions_[{i, symbol}] = index;
        }
      }
    }
  } while (old_size != states_.size());
}

Set<char> LR1Parser::First_(const std::string& expression) const {
  if (expression.empty()) {
    return {'\0'};
  }
  if (IsTerminal_(expression[0])) {
    return {expression[0]};
  }
  Set<char> result{};
  if (IsNonTerminal_(expression[0])) {
    Set<char> processing_nonterminals{expression[0]};
    Set<char> processed_nonterminals;
    do {
      Set<char> new_nonterminals;
      for (const char nonterminal : processing_nonterminals) {
        processed_nonterminals.insert(nonterminal);
        for (const auto& production_rule : production_rules_) {
          if (production_rule.first == nonterminal) {
            if (IsNonTerminal_(production_rule.second[0])) {
              if (!processed_nonterminals.contains(nonterminal)) {
                new_nonterminals.insert(production_rule.second[0]);
              }
            } else if (IsTerminal_(production_rule.second[0])){
              result.insert(production_rule.second[0]);
            }
          }
        }
      }
      processing_nonterminals = new_nonterminals;
    } while (!processing_nonterminals.empty());
  }
  return result;
}

bool LR1Parser::IsTerminal_(const char symbol) const {
  return terminals_.contains(symbol);
}

Set<Situation> LR1Parser::Closure_(const Set<Situation>& situations) const {
  Set<Situation> result = situations;
  Set<Situation> processing_situations = situations;
  do {
    Set<Situation> new_situations;
    for (const auto& situation : processing_situations) {
      int index = situation.next_symbol_index;
      auto cur_production_rule = situation.production_rule;
      if (IsNonTerminal_(cur_production_rule.second[index])) {
        char cur_nonterminal = cur_production_rule.second[index];
        auto cur_rhs_substr = cur_production_rule.second.substr(index + 1);
        if (situation.expected_symbol != '\0') {
          cur_rhs_substr += situation.expected_symbol;
        }
        for (const auto& production_rule : production_rules_) {
          if (production_rule.first == cur_nonterminal) {
            for (char terminal : First_(cur_rhs_substr)) {
              if (!result.contains({production_rule, 0, terminal})) {
                new_situations.insert({production_rule, 0, terminal});
              }
            }
          }
        }
      }
    }
    result.insert(new_situations.begin(), new_situations.end());
    processing_situations = new_situations;
  } while(!processing_situations.empty());
  return result;
}

bool LR1Parser::IsNonTerminal_(const char symbol) const {
  return nonterminals_.contains(symbol);
}

Set<Situation> LR1Parser::Goto_(const Set<Situation>& situations,
                                const char symbol) const {
  auto result = Set<Situation>{};
  for (auto situation : situations) {
    int index = situation.next_symbol_index;
    if (situation.production_rule.second[index] == symbol) {
      ++situation.next_symbol_index;
      result.insert(situation);
    }
  }
  return Closure_(result);
}

void LR1Parser::Clear_() {
  actions_.clear();
  states_.clear();
  nonterminals_.clear();
  terminals_.clear();
  production_rules_.clear();
}