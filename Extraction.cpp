// llvm & clang includes
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

// standard includes
#include <iostream>

// custom includes
#include "Extraction.h"

// namespaces
using namespace clang; // CompilerInstance, ASTFrontendAction, ASTConsumer
using namespace clang::ast_matchers; // StatementMatcher, actual AST matchers like forStmt etc.
using namespace clang::tooling; // CommonOptionsParser


//-----------------------------------------------------------------------------

//ctor
Extractor::Extractor(std::string id, StatementMatcher Matcher) : id(id), Matcher(Matcher){
    this->appearances=0;
}
//
void Extractor::run(const MatchFinder::MatchResult &Result) {
    std::cout << "\033[32mFound match in AST\033[0m" << std::endl;
    const Stmt* node = Result.Nodes.getNodeAs<clang::Stmt>(id);
    ASTContext* Context = Result.Context;
    appearances++;

    // if(node){
    //     FullSourceLoc Location = Context->getFullLoc(node->getBeginLoc());
    //     if(Location.isValid()){
    //         int LineNumber = Location.getLineNumber();
    //         result = LineNumber;
    //
    //     }
    // }
}

// TODO: put this method into Extract class
int extract(std::string id, StatementMatcher Matcher, ClangTool Tool){
    std::cout << "extract called" << std::endl;
    Extractor extractor(id, Matcher);
    MatchFinder Finder;
    Finder.addMatcher(Matcher, &extractor);
    Tool.run(newFrontendActionFactory(&Finder).get());
    return extractor.appearances;

}

//-----------------------------------------------------------------------------
