#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <iostream>

#include "cxx-langstat/MatchingExtractor.h"

using namespace clang; // CompilerInstance, ASTFrontendAction, ASTConsumer
using namespace clang::ast_matchers; // StatementMatcher, actual AST matchers like forStmt etc.
using namespace clang::tooling; // CommonOptionsParser

//-----------------------------------------------------------------------------

template<typename T>
Match<T>::Match(unsigned location, const T* node, ASTContext* ctxt) : location(location), node(node), ctxt(ctxt){
    // std::cout<<"Match ctor"<<std::endl;
}

//-----------------------------------------------------------------------------

// ctor
template<typename T>
MatchingExtractor<T>::MatchingExtractor(std::string id) : matcherid(id){
    std::cout<<"MatchingExtractor ctor"<<std::endl;
}
template<typename T>
void MatchingExtractor<T>::run(const MatchFinder::MatchResult &Result) {
    // maybe need template or switch statement? could also put 'kind' of node into id to trigger right code
    // one could do this structure of if statement to check what it is
    // one would however go from more to less specific to see what it is
    if(const T* node = Result.Nodes.getNodeAs<T>(this->matcherid)) {
        ASTContext* Context = Result.Context;
        unsigned Location = Context->getFullLoc(node->getBeginLoc()).getLineNumber();
        // const char* StmtKind = node->getStmtClassName();
        Match<T> m(Location, node, Context);
        matches.emplace_back(m);
    } else {
        std::cout << "Error extracting type" << std::endl;
    }
}
template<typename T>
void MatchingExtractor<T>::resetState(){
    matches.clear(); // does this ensure no memory leak?
}

//-----------------------------------------------------------------------------

// request instantiations of MatchingExtractor template s.t. linker can find them
// bad: have to do this for every instantiation needed
template class MatchingExtractor<clang::Stmt>;
template class MatchingExtractor<clang::Decl>;

// not necessary, because constructed by MatchingExtractor
// template struct Match<clang::Stmt>;
// template struct Match<clang::Decl>;

//-----------------------------------------------------------------------------
