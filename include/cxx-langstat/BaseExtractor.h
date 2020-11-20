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
    template<typename NodeType>
    Matches<NodeType>
    extract(std::string id,
        clang::ast_matchers::internal::Matcher<NodeType> Matcher);
    template<typename NodeType, typename ...Types>
    std::array<Matches<NodeType>, sizeof...(Types)>
    extract2(clang::ast_matchers::internal::Matcher<NodeType> Matcher,
        Types... ids);
private:
    clang::ASTContext& Context;
};

// Implementations s.t. explicit instantiation is not necessary
// Might move this to a BaseExtractor.tpp file
template<typename NodeType>
Matches<NodeType>
BaseExtractor::extract(std::string id,
    clang::ast_matchers::internal::Matcher<NodeType> Matcher){
        MatchingExtractor<NodeType, const char*> extr(id);
        clang::ast_matchers::MatchFinder Finder;
        Finder.addMatcher(Matcher, &extr);
        Finder.matchAST(Context);
        return extr.matches[0];
}
template<typename NodeType, typename ...Types>
std::array<Matches<NodeType>, sizeof...(Types)>
BaseExtractor::extract2(clang::ast_matchers::internal::Matcher<NodeType> Matcher,
    Types... ids){
        MatchingExtractor<NodeType, Types...> extr(ids...);
        clang::ast_matchers::MatchFinder Finder;
        Finder.addMatcher(Matcher, &extr);
        Finder.matchAST(Context);
        return extr.matches;
}

//-----------------------------------------------------------------------------

#endif // BASEEXTRACTOR_H
