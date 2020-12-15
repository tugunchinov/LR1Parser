#include "Grammar.h"
#include "gtest/gtest.h"
#include "LR1Parser.h"

class TestEnvironment: public ::testing::Environment {
 public:
  static Grammar GetBraceGrammar() {
    Set<char> terminals = {'a', 'b'};
    Set<char> nonterminals = {'S'};
    Set<ProductionRule> production_rules = {
        {'S', "aSbS"},
        {'S', ""}
    };
    return Grammar(terminals, nonterminals, production_rules, 'S');
  }
  static Grammar GetStrangeGrammar() {
    Set<char> terminals = {'d', 'c'};
    Set<char> nonterminals = {'S', 'C'};
    Set<ProductionRule> production_rules = {
        {'S', "CC"},
        {'C', "cC"},
        {'C', "d"}
    };
    return Grammar(terminals, nonterminals, production_rules, 'S');
  }
  void SetUp() override = 0;
};

class ParseTest: public ::testing::Test {
 protected:
  void SetUp() override {
    brace_grammar = TestEnvironment::GetBraceGrammar();
    strange_grammar = TestEnvironment::GetStrangeGrammar();
  }
  Grammar brace_grammar;
  Grammar strange_grammar;
  LR1Parser parser;
};

TEST_F(ParseTest, ParseBraces) {
  parser.Fit(brace_grammar);
  EXPECT_TRUE(parser.Predict("aabb"));
  EXPECT_TRUE(parser.Predict(""));
  EXPECT_TRUE(parser.Predict("ababab"));
  EXPECT_TRUE(parser.Predict("ab"));
  EXPECT_TRUE(parser.Predict("aaabbbabab"));
  EXPECT_FALSE(parser.Predict("abba"));
  EXPECT_FALSE(parser.Predict("a"));
  EXPECT_FALSE(parser.Predict("b"));
  EXPECT_FALSE(parser.Predict("bb"));
  EXPECT_FALSE(parser.Predict("aa"));
  EXPECT_FALSE(parser.Predict("bbbaaa"));
  EXPECT_FALSE(parser.Predict("ba"));
}

TEST_F(ParseTest, ParseStrane) {
  parser.Fit(strange_grammar);
  EXPECT_TRUE(parser.Predict("dd"));
  EXPECT_TRUE(parser.Predict("ccccccdd"));
  EXPECT_FALSE(parser.Predict("c"));
  EXPECT_FALSE(parser.Predict("cc"));
}

//TODO: More tests