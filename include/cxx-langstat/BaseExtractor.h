#ifndef BASEEXTRACTOR_H
#define BASEEXTRACTOR_H

#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "cxx-langstat/MatchingExtractor.h"

//-----------------------------------------------------------------------------

class BaseExtractor{
public :
    // ctor, dtor
    BaseExtractor(clang::ASTContext& Context);
    // Responsible for invoking appropriate concrete Extractor
    Matches<clang::Stmt> extract(std::string id,
        clang::ast_matchers::StatementMatcher Matcher);
    Matches<clang::Decl> extract(std::string id,
        clang::ast_matchers::DeclarationMatcher Matcher);
    template<typename M, typename ...Types>
    std::array<Matches<clang::Decl>, sizeof...(Types)> extract2(M Matcher,
        Types... ids);
private:
    clang::ASTContext& Context;
};

//-----------------------------------------------------------------------------

#endif // BASEEXTRACTOR_H
