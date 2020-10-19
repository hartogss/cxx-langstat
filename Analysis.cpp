#include "Analysis.h"

using namespace clang::ast_matchers;


// Definitions for class 'Analysis'
// Empty analysis ctor
Analysis::Analysis() : Matcher(allOf(hasAncestor(forStmt()), unless(hasAncestor(forStmt())))){
}
// ctor
Analysis::Analysis(StatementMatcher Matcher, std::function<void()> Runner) : Matcher(Matcher), Runner(Runner) {
}
//
clang::ast_matchers::StatementMatcher Analysis::getMatcher(){
    return Matcher;
}
