#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <iostream>

#include "cxx-langstat/BaseExtractor.h"
#include "cxx-langstat/MatchingExtractor.h"

using namespace clang; // CompilerInstance, ASTFrontendAction, ASTConsumer
using namespace clang::ast_matchers; // StatementMatcher, actual AST matchers like forStmt etc.
using namespace clang::tooling; // CommonOptionsParser

//-----------------------------------------------------------------------------

// ctor
BaseExtractor::BaseExtractor(clang::tooling::ClangTool Tool) : Tool(Tool){
    std::cout<<"BaseExtractor ctor"<<std::endl;
}
Matches<clang::Stmt> BaseExtractor::extract(std::string id, StatementMatcher Matcher){
    MatchingExtractor<Stmt> extr(Tool, id);
    MatchFinder Finder;
    Finder.addMatcher(Matcher, &extr);
    Tool.run(newFrontendActionFactory(&Finder).get());
    return extr.matches;
}
Matches<clang::Decl> BaseExtractor::extract(std::string id, DeclarationMatcher Matcher){
    MatchingExtractor<Decl> extr(Tool, id);
    MatchFinder Finder;
    Finder.addMatcher(Matcher, &extr);
    Tool.run(newFrontendActionFactory(&Finder).get());
    return extr.matches;
}

//-----------------------------------------------------------------------------
