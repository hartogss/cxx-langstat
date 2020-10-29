#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <iostream>

#include "Extraction.h"

// namespaces
using namespace clang; // CompilerInstance, ASTFrontendAction, ASTConsumer
using namespace clang::ast_matchers; // StatementMatcher, actual AST matchers like forStmt etc.
using namespace clang::tooling; // CommonOptionsParser


//-----------------------------------------------------------------------------

Gist::Gist() : numMatches(0), locations(){
    std::cout<<"Gist ctor"<<std::endl;
}
Gist::~Gist(){
    std::cout<<"Gist dtor"<<std::endl;
}

//-----------------------------------------------------------------------------

// ctor
Extractor::Extractor(clang::tooling::ClangTool Tool) : Tool(Tool){
    std::cout<<"Extractor ctor"<<std::endl;
}
Extractor::~Extractor(){
    std::cout<<"Extractor dtor"<<std::endl;
}
void Extractor::run(const MatchFinder::MatchResult &Result) {
    // std::cout << "\033[32mFound match in AST\033[0m" << std::endl;
    const Stmt* node = Result.Nodes.getNodeAs<clang::Stmt>(this->matcherid);
    ASTContext* Context = Result.Context;
    gist.numMatches++;

    if(node){
        FullSourceLoc Location = Context->getFullLoc(node->getBeginLoc());
        if(Location.isValid()){
            gist.locations.emplace_back(Location.getLineNumber());
        }
    }
}
void Extractor::resetState(){
    gist.numMatches=0;
    gist.locations.clear();
}
Gist Extractor::extract(std::string id, StatementMatcher Matcher){
    resetState();
    MatchFinder Finder;
    this->matcherid=id;
    Finder.addMatcher(Matcher, this);
    Tool.run(newFrontendActionFactory(&Finder).get());
    return gist; //according to my understanding this returns a copy to caller, how is that copy constructed at the caller?
}
