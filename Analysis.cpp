#include "Analysis.h"

using namespace clang::ast_matchers;


// Definitions for class 'Analysis'
// Empty analysis ctor
Analysis::Analysis() : Matcher(allOf(hasAncestor(forStmt()), unless(hasAncestor(forStmt())))){
}
// ctor
Analysis::Analysis(std::string Name, StatementMatcher Matcher,
    std::function<void(const clang::Stmt*)> Runner) :
    Name(Name),
    Matcher(Matcher),
    Runner(Runner) {
}
//
clang::ast_matchers::StatementMatcher Analysis::getMatcher(){
    return Matcher;
}
