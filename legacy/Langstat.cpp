#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"

#include <iostream>

#include "cxx-langstat/LoopDepthAnalysis.h"
#include "cxx-langstat/TestAnalysis.h"
#include "cxx-langstat/CyclomaticComplexityAnalysis.h"
#include "cxx-langstat/LoopKindAnalysis.h"
#include "cxx-langstat/UsingAnalysis.h"

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

    LoopDepthAnalysis LDA(Tool, FSAMaxDepthOption);
    LDA.run();

    CyclomaticComplexityAnalysis CCA(Tool);
    CCA.run();

    // TestAnalysis T(Tool);
    // T.run();

    LoopKindAnalysis LKA(Tool);
    LKA.run();

    UsingAnalysis UA(Tool);
    UA.run();


    return 0;
}
