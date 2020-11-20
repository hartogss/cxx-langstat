#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <iostream>

#include "cxx-langstat/BaseExtractor.h"
#include "cxx-langstat/MatchingExtractor.h"

using namespace clang;
using namespace clang::ast_matchers; // StatementMatcher, actual AST matchers like forStmt etc.

//-----------------------------------------------------------------------------

// ctor
BaseExtractor::BaseExtractor(clang::ASTContext& Context) : Context(Context){
    std::cout<<"BaseExtractor ctor"<<std::endl;
}
Matches<Stmt> BaseExtractor::extract(std::string id,
    StatementMatcher Matcher){
        MatchingExtractor<Stmt, const char*> extr(id);
        MatchFinder Finder;
        Finder.addMatcher(Matcher, &extr);
        Finder.matchAST(Context);
        return extr.matches[0];
}
Matches<Decl> BaseExtractor::extract(std::string id,
    DeclarationMatcher Matcher){
        MatchingExtractor<Decl, const char*> extr(id);
        MatchFinder Finder;
        Finder.addMatcher(Matcher, &extr);
        Finder.matchAST(Context);
        return extr.matches[0];
}

//-----------------------------------------------------------------------------
