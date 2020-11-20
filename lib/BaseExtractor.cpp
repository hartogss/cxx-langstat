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
template<typename MatcherType, typename ...Types>
std::array<Matches<Decl>, sizeof...(Types)>
BaseExtractor::extract2(MatcherType Matcher, Types... ids){
        MatchingExtractor<Decl, Types...> extr(ids...);
        MatchFinder Finder;
        Finder.addMatcher(Matcher, &extr);
        Finder.matchAST(Context);
        return extr.matches;
}

template
std::array<Matches<clang::Decl>, 2> BaseExtractor::extract2<DeclarationMatcher, const char*, const char*>(DeclarationMatcher,
    const char*, const char*);
template
std::array<Matches<clang::Decl>, 3> BaseExtractor::extract2<DeclarationMatcher, const char*, const char*, const char*>(DeclarationMatcher,
    const char*, const char*, const char*);

//-----------------------------------------------------------------------------
