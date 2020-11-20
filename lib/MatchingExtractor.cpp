#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <iostream>

#include "cxx-langstat/MatchingExtractor.h"

using namespace clang; // CompilerInstance, ASTFrontendAction, ASTConsumer
using namespace clang::ast_matchers; // StatementMatcher, actual AST matchers like forStmt etc.

//-----------------------------------------------------------------------------

template<typename T>
Match<T>::Match(unsigned location, const T* node, clang::ASTContext* ctxt) :
    location(location),
    node(node),
    ctxt(ctxt){
}

//-----------------------------------------------------------------------------

template<typename T, typename ...Types>
MatchingExtractor<T, Types...>::MatchingExtractor(std::string id) : matcherids(){
    matcherids.emplace_back(id);
    std::cout<<"MatchingExtractor ctor"<<std::endl;
}
template<typename ...Types>
auto construct(Types... ids){
    if constexpr(sizeof...(Types) > 0){
        return std::vector<std::string>{ids...};
    } else
        return std::vector<std::string>();
}

template<typename T, typename ...Types>
MatchingExtractor<T, Types...>::MatchingExtractor(Types... ids) :
    matches(std::array<Matches<T>, sizeof...(Types)>()),
    matcherids(construct(ids...)){
        std::cout<<"MatchingExtractor special ctor"<<std::endl;
}
template<typename T, typename ...Types>
void MatchingExtractor<T, Types...>::run(const MatchFinder::MatchResult &Result) {
    for (unsigned pos=0; pos<matcherids.size(); pos++) {
        auto matcherid = matcherids[pos];
        if(const T* node = Result.Nodes.getNodeAs<T>(matcherid)) {
            ASTContext* Context = Result.Context;
            unsigned Location = Context->getFullLoc(node->getBeginLoc()).getLineNumber();
            // const char* StmtKind = node->getStmtClassName();
            Match<T> m(Location, node, Context);
            matches[pos].emplace_back(m);
        } else {
            std::cout << "Error extracting type" << std::endl;
        }
    }
}
template<typename T, typename ...Types>
void MatchingExtractor<T, Types...>::resetState(){
    // matches.clear(); // does this ensure no memory leak?
}

//-----------------------------------------------------------------------------

// Request instantiations of MatchingExtractor template s.t. linker can find them
// Bad: have to do this for every instantiation needed
// This is called explicit instantiation
template class MatchingExtractor<Stmt, const char*>;
template class MatchingExtractor<Stmt, const char*, const char*>;
template class MatchingExtractor<Stmt, const char*, const char*, const char*>;
template class MatchingExtractor<Decl, const char*>;
template class MatchingExtractor<Decl, const char*, const char*>;
template class MatchingExtractor<Decl, const char*, const char*, const char*>;


// not necessary, because constructed by MatchingExtractor
// template struct Match<clang::Stmt>;
// template struct Match<clang::Decl>;

//-----------------------------------------------------------------------------
