#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"

#include <iostream>

#include "cxx-langstat/ForStmtAnalysis.h"
#include "cxx-langstat/TestAnalysis.h"


// namespaces
using namespace clang; // CompilerInstance, ASTFrontendAction, ASTConsumer
using namespace clang::tooling; // CommonOptionsParser


//-----------------------------------------------------------------------------
// CL options

// Options in CLI specific/nongeneric to clang-stat
llvm::cl::OptionCategory ClangStatCategory("clang-stat options");
llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
llvm::cl::extrahelp MoreHelp("\nMore help text coming soon...\n");

// Analysis-specific options
llvm::cl::opt<int> FSAMaxDepthOption(
    "forstmt",
    llvm::cl::desc("Whether we want to catch for statements "),
    llvm::cl::cat(ClangStatCategory)
);

//-----------------------------------------------------------------------------


int main(int argc, const char** argv){
    // parses all options that command-line tools have in common
    CommonOptionsParser Parser(argc, argv, ClangStatCategory);

    // Print all file name & locations
    for (auto SourceFilePath : Parser.getSourcePathList()){
        std::cout << SourceFilePath << std::endl;
    }

    ClangTool Tool(Parser.getCompilations(), Parser.getSourcePathList());

    ForStmtAnalysis A(Tool, FSAMaxDepthOption);
    A.run();

    return 0;
}
