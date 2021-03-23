#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <iostream>

#include "cxx-langstat/MatchingExtractor.h"

using namespace clang;
using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------

void MatchingExtractor::run(const MatchFinder::MatchResult& Result) {
    Results.emplace_back(Result);
}

//-----------------------------------------------------------------------------
