#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <iostream>

#include "cxx-langstat/MatchingExtractor.h"

using namespace clang; // CompilerInstance, ASTFrontendAction, ASTConsumer
using namespace clang::ast_matchers; // StatementMatcher, actual AST matchers like forStmt etc.

//-----------------------------------------------------------------------------

MatchingExtractor::MatchingExtractor(){
    // std::cout<<"MatchingExtractor ctor"<<std::endl;
}
MatchingExtractor::~MatchingExtractor(){
    // std::cout<<"MatchingExtractor dtor"<<std::endl;
}

void MatchingExtractor::run(const MatchFinder::MatchResult& Result) {
    Results.emplace_back(Result);
}

//-----------------------------------------------------------------------------
