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

// ctor
Extractor::Extractor(clang::tooling::ClangTool Tool) : Tool(Tool){
    this->NumMatches=0;
}
void Extractor::run(const MatchFinder::MatchResult &Result) {
    // std::cout << "\033[32mFound match in AST\033[0m" << std::endl;
    const Stmt* node = Result.Nodes.getNodeAs<clang::Stmt>(this->matcherid);
    ASTContext* Context = Result.Context;
    this->NumMatches++;

    /*if(node){
        FullSourceLoc Location = Context->getFullLoc(node->getBeginLoc());
        if(Location.isValid()){
            int LineNumber = Location.getLineNumber();
            result = LineNumber;

        }
    }*/
}

unsigned Extractor::resetState(){
    unsigned result = this->NumMatches;
    this->NumMatches=0;
    return result;
}
int Extractor::extract(std::string id, StatementMatcher Matcher){
    MatchFinder Finder;
    this->matcherid=id;
    Finder.addMatcher(Matcher, this);
    Tool.run(newFrontendActionFactory(&Finder).get());
    return resetState();
}
