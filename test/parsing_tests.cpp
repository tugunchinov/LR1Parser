#include "Grammar.h"
#include "gtest/gtest.h"
#include "LR1Parser.h"

class TestEnvironment: public ::testing::Environment {
 public:
  static Grammar GetBraceGrammar() {
    Set<char> terminals = {'a', 'b'};
    Set<char> nonterminals = {'S'};
    std::vector<ProductionRule> production_rules = {
        {'S', "aSbS"},
        {'S', ""}
    };
    return Grammar(terminals, nonterminals, production_rules, 'S');
  }
  static Grammar GetStrangeGrammar() {
    Set<char> terminals = {'d', 'c'};
    Set<char> nonterminals = {'S', 'C'};
    std::vector<ProductionRule> production_rules = {
        {'S', "CC"},
        {'C', "cC"},
        {'C', "d"}
    };
    return Grammar(terminals, nonterminals, production_rules, 'S');
  }
  static Grammar GetMathGrammar() {
    Set<char> terminals = {'x', 'y', 'z', '(', ')', '+', '*'};
    Set<char> nonterminals = {'S', 'P', 'T'};
    std::vector<ProductionRule> production_rules = {
        {'S', "S+P"},
        {'S', "P"},
        {'P', "P*T"},
        {'P', "T"},
        {'T', "(S)"},
        {'T', "x"},
        {'T', "y"},
        {'T', "z"}
    };
    return Grammar(terminals, nonterminals, production_rules, 'S');
  }
    static Grammar GetRecursiveGrammar() {
      Set<char> terminals = {'a', 'b'};
      Set<char> nonterminals = {'S', 'T'};
      std::vector<ProductionRule> production_rules = {
          {'S', "Ta"},
          {'T', "Sb"},
          {'S', ""}
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
    math_grammar = TestEnvironment::GetMathGrammar();
    recursive_grammar = TestEnvironment::GetRecursiveGrammar();
  }
  Grammar brace_grammar;
  Grammar strange_grammar;
  Grammar math_grammar;
  Grammar recursive_grammar;
  LR1Parser parser;
};

TEST_F(ParseTest, ParseBraces) {
  parser.Fit(brace_grammar);
  EXPECT_TRUE(parser.Predict("aabb"));
  EXPECT_TRUE(parser.Predict(""));
  EXPECT_TRUE(parser.Predict("ababab"));
  EXPECT_TRUE(parser.Predict("aaabbabb"));
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

TEST_F(ParseTest, ParseStrange) {
  parser.Fit(strange_grammar);
  EXPECT_TRUE(parser.Predict("dd"));
  EXPECT_TRUE(parser.Predict("ccccccdd"));
  EXPECT_FALSE(parser.Predict("c"));
  EXPECT_FALSE(parser.Predict("cc"));
}

TEST_F(ParseTest, ParseMath) {
  parser.Fit(math_grammar);
  EXPECT_TRUE(parser.Predict("x"));
  EXPECT_TRUE(parser.Predict("x+z"));
  EXPECT_TRUE(parser.Predict("((((((((((x))))))))))"));
  EXPECT_TRUE(parser.Predict("x+(y+(x+(z+x)))"));
  EXPECT_TRUE(parser.Predict("x+x*y+x*y*z+(x*(x*(x*(y+z))))"));
  EXPECT_TRUE(parser.Predict("x*((y+z)*z+(x*y+(x+y*z)*(x+y)))"));
  EXPECT_FALSE(parser.Predict("x+"));
  EXPECT_FALSE(parser.Predict("x+(y+z"));
  EXPECT_FALSE(parser.Predict("x+y*)z("));
  EXPECT_FALSE(parser.Predict("x+y*)z("));
  EXPECT_FALSE(parser.Predict("(((((((((x((((((((("));
}

TEST_F(ParseTest, ParseRecursive) {
  parser.Fit(recursive_grammar);
  EXPECT_TRUE(parser.Predict("ba"));
  EXPECT_TRUE(parser.Predict("baba"));
  EXPECT_TRUE(parser.Predict("bababa"));
}