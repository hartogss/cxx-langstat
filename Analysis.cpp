#include "Analysis.h"

using namespace clang::ast_matchers;


// Definitions for class 'Analysis'
// Empty analysis ctor
Analysis::Analysis() : BaseMatcher(allOf(hasAncestor(forStmt()), unless(hasAncestor(forStmt())))){
}
// ctor
Analysis::Analysis(std::string Name, StatementMatcher BaseMatcher,
    std::function<void(const MatchFinder::MatchResult&)> Runner) :
    Name(Name),
    BaseMatcher(BaseMatcher),
    Runner(Runner) {
}
//
clang::ast_matchers::StatementMatcher Analysis::getBaseMatcher(){
    return BaseMatcher;
}
