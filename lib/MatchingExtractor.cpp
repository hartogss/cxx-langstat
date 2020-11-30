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

MatchingExtractor::MatchingExtractor(){
    std::cout<<"MatchingExtractor ctor"<<std::endl;
}

void MatchingExtractor::run(const MatchFinder::MatchResult &Result) {
    Results.emplace_back(Result);
}
void MatchingExtractor::resetState(){
    // matches.clear(); // does this ensure no memory leak?
}

//-----------------------------------------------------------------------------

// Request instantiations of MatchingExtractor template s.t. linker can find them
// Bad: have to do this for every instantiation needed
// This is called explicit instantiation
// template class MatchingExtractor<Stmt>;
// template class MatchingExtractor<Decl>;


template struct Match<clang::Stmt>;
template struct Match<clang::Decl>;

//-----------------------------------------------------------------------------
