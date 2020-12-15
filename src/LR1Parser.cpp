#include <stack>
#include <stdexcept>

#include "LR1Parser.h"

void LR1Parser::Fit(const Grammar& grammar) {
  Clear_();
  Situation end_situation = Init_(grammar);
  MakeStates_(grammar);
  for (int i = 0; i < states_.size(); ++i) {
    if (states_[i].contains(end_situation)) {
      actions_[{i, '\0'}] = Accepted{};
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
  production_rules_.insert(start_rule);
  Situation start_situation{start_rule, 0, '\0'};
  Situation end_situation{start_rule, 1, '\0'};
  nonterminals_ = grammar.GetNonTerminals();
  terminals_ = grammar.GetTerminals();
  State start_state = Closure_({start_situation});
  states_.push_back(start_state);
  return end_situation;
}

bool LR1Parser::Predict(const std::string& word) const {
  std::stack<std::variant<char, int>> stack;
  stack.push(0);
  int pos = 0;
  while (true) {
    char current_symbol = word[pos];
    int state = std::get<int>(stack.top());
    if (!actions_.contains({state, current_symbol})) {
      return false;
    }
    if (actions_.at({state, current_symbol}).index() == INT) {
      stack.push(current_symbol);
      stack.push(std::get<int>(actions_.at({state, current_symbol})));
      ++pos;
    } else if (actions_.at({state, current_symbol}).index() == PRODUCTION_RULE) {
      auto production_rule =
           std::get<ProductionRule>(actions_.at({state, current_symbol}));
      for (int i = 0; i < 2*production_rule.second.size(); ++i) {
        stack.pop();
      }
      int new_state = std::get<int>(stack.top());
      stack.push(production_rule.first);
      stack.push(std::get<int>(actions_.at({new_state, production_rule.first})));
    } else if (actions_.at({state, current_symbol}).index() == ACCEPTED) {
      return true;
    }
  }
}

void LR1Parser::MakeStates_(const Grammar& grammar) {
  auto symbols = nonterminals_;
  symbols.insert(terminals_.begin(), terminals_.end());
  bool states_changing = true;
  while (states_changing) {
    states_changing = false;
    for (int i = 0; i < states_.size(); ++i) {
      for (char symbol : symbols) {
        auto new_state = Goto_(states_[i], symbol);
        if (!new_state.empty()) {
          int index = std::find(states_.begin(),
                                states_.end(),
                                new_state) - states_.begin();
          if (index == states_.size()) {
            states_changing = true;
            states_.push_back(new_state);
          }
          actions_[{i, symbol}] = index;
        }
      }
    }
  }
}

Set<char> LR1Parser::First_(const std::string& expression) const {
  if (expression.empty()) {
    return {'\0'};
  }
  if (terminals_.contains(expression[0])) {
    return {expression[0]};
  }
  Set<char> result{};
  if (nonterminals_.contains(expression[0])) {
    for (const auto& production_rule : production_rules_) {
      if (production_rule.first == expression[0]) {
        auto first = First_(production_rule.second);
        result.insert(first.begin(), first.end());
      }
    }
  } else {
    std::string error_message = "Symbol ";
    error_message += expression[0];
    error_message += " isn't recognized";
    throw std::invalid_argument(error_message);
  }
  return result;
}

Set<Situation> LR1Parser::Closure_(const Set<Situation>& situations) const {
  Set<Situation> result = situations;
  bool situations_changing = true;
  while (situations_changing) {
    situations_changing = false;
    for (const auto& situation : result) {
      int index = situation.next_symbol_index;
      auto cur_production_rule = situation.production_rule;
      if (nonterminals_.contains(cur_production_rule.second[index])) {
        char cur_nonterminal = cur_production_rule.second[index];
        auto cur_rhs_substr = cur_production_rule.second.substr(index + 1);
        if (situation.expected_symbol != '\0') {
          cur_rhs_substr += situation.expected_symbol;
        }
        for (const auto& production_rule : production_rules_) {
          if (production_rule.first == cur_nonterminal) {
            for (char terminal : First_(cur_rhs_substr)) {
              if (!result.contains({production_rule, 0, terminal})) {
                result.insert({production_rule, 0, terminal});
                situations_changing = true;
              }
            }
          }
        }
      }
    }
  }
  return result;
}

Set<Situation> LR1Parser::Goto_(const Set<Situation>& situations,
                                const char symbol) const {
  auto result = Set<Situation>{};
  for (auto situation : situations) {
    auto pos = situation.production_rule.second.find(symbol);
    while (pos != std::string::npos) {
      if (situation.next_symbol_index == pos) {
        situation.next_symbol_index = pos + 1;
        result.insert(situation);
      } else {
        pos = situation.production_rule.second.find(symbol, pos + 1);
      }
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