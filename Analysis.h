#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

class Analysis {
public:
    // Empty analysis ctor
    Analysis();
    // ctor
    Analysis(std::string name,
            clang::ast_matchers::StatementMatcher BaseMatcher,
            std::function<void(const clang::ast_matchers::MatchFinder::MatchResult&)> Runner);
    //
    clang::ast_matchers::StatementMatcher getBaseMatcher();
    //
    std::string Name;
    // should become more general for example to support analyses that rely on
    // something else than matchers -> maybe with inheritance/templates
    clang::ast_matchers::StatementMatcher BaseMatcher;
    // Runner: defines what happens when matchcallback is called on a match
    // template <typename T> std::function<void(T)> Runner;
    std::function<void(const clang::ast_matchers::MatchFinder::MatchResult&)> Runner;
};

#endif /* ANALYSIS_H */
