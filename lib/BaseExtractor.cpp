#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <iostream>

#include "cxx-langstat/BaseExtractor.h"
#include "cxx-langstat/MatchingExtractor.h"

using namespace clang;
using namespace clang::ast_matchers; // StatementMatcher, actual AST matchers like forStmt etc.

//-----------------------------------------------------------------------------

BaseExtractor::BaseExtractor(){
    std::cout<<"BaseExtractor ctor"<<std::endl;
}

//-----------------------------------------------------------------------------
