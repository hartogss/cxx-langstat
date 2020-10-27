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


llvm::cl::OptionCategory ClangStatCategory("clang-stat options");
llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
llvm::cl::extrahelp MoreHelp("\nMore help text coming soon...\n");

int main(int argc, const char** argv){
    // parses all options that command-line tools have in common
    CommonOptionsParser Parser(argc, argv, ClangStatCategory);

    // Print all file name & locations
    for (auto SourceFilePath : Parser.getSourcePathList()){
        std::cout << SourceFilePath << std::endl;
    }

    ClangTool Tool(Parser.getCompilations(), Parser.getSourcePathList());


    //for now build analysis: simplified: extraction & statistics computation in main
    // extraction
    std::string matcherid = "fs";
    StatementMatcher Matcher = forStmt().bind(matcherid);
    int numMatches = extract(matcherid, Matcher, Tool);
    std::cout << "Number of matches " << numMatches << "\n";


    return 0;
}
