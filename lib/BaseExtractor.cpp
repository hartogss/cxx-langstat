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
        MatchingExtractor<Stmt> extr(id);
        MatchFinder Finder;
        Finder.addMatcher(Matcher, &extr);
        Finder.matchAST(Context);
        return extr.matches[0];
}
Matches<Decl> BaseExtractor::extract(std::string id,
    DeclarationMatcher Matcher){
        MatchingExtractor<Decl> extr(id);
        MatchFinder Finder;
        Finder.addMatcher(Matcher, &extr);
        Finder.matchAST(Context);
        return extr.matches[0];
}
template<typename M, typename ...Types>
std::array<Matches<clang::Decl>, sizeof...(Types)>
BaseExtractor::extract2(M Matcher,
    Types... ids){
        MatchingExtractor<Decl> extr(ids...);
        MatchFinder Finder;
        Finder.addMatcher(Matcher, &extr);
        Finder.matchAST(Context);
        return extr.matches;
}

template
std::array<Matches<clang::Decl>, 2> BaseExtractor::extract2<DeclarationMatcher, char const*, char const*>(DeclarationMatcher,
    char const*, char const*);

//-----------------------------------------------------------------------------
