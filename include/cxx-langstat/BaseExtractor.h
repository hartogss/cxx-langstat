#ifndef BASEEXTRACTOR_H
#define BASEEXTRACTOR_H

#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "cxx-langstat/MatchingExtractor.h"
#include "cxx-langstat/Utils.h"

//-----------------------------------------------------------------------------

class BaseExtractor{
public :
    // ctor, dtor
    BaseExtractor();
    // Responsible for invoking appropriate concrete Extractor
    template<typename NodeType>
    Matches<NodeType>
    extract(clang::ASTContext& Context, std::string id,
        clang::ast_matchers::internal::Matcher<NodeType> Matcher);
    template<typename NodeType>
    std::vector<clang::ast_matchers::MatchFinder::MatchResult >
    extract2(clang::ASTContext& Context, clang::ast_matchers::internal::Matcher<NodeType> Matcher);
};

// Implementations s.t. explicit instantiation is not necessary
// Might move this to a BaseExtractor.tpp file
template<typename NodeType>
Matches<NodeType>
BaseExtractor::extract(clang::ASTContext& Context, std::string id,
    clang::ast_matchers::internal::Matcher<NodeType> Matcher){
        auto Results = this->extract2(Context, Matcher);
        return getASTNodes<NodeType>(Results, id);
}
template<typename NodeType>
std::vector<clang::ast_matchers::MatchFinder::MatchResult>
BaseExtractor::extract2(clang::ASTContext& Context, clang::ast_matchers::internal::Matcher<NodeType> Matcher){
        MatchingExtractor Callback;
        clang::ast_matchers::MatchFinder Finder;
        Finder.addMatcher(Matcher, &Callback);
        Finder.matchAST(Context);
        return Callback.Results;
}

//-----------------------------------------------------------------------------

#endif // BASEEXTRACTOR_H
