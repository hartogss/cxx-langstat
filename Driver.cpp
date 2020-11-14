// helpful: https://github.com/peter-can-talk/cppnow-2017/blob/master/code/mccabe/mccabe.cpp

// clang includes
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"

// standard includes
#include <iostream>

//
#include "cxx-langstat/CyclomaticComplexityAnalysis.h"
#include "cxx-langstat/LoopDepthAnalysis.h"
#include "cxx-langstat/LoopKindAnalysis.h"
#include "cxx-langstat/UsingAnalysis.h"

using namespace clang;
using namespace clang::ast_matchers;


// Consumes the AST, i.e. does computations on it
class Consumer : public ASTConsumer {
public:
    // Constructor, how is finder & handler constructed?
    Consumer(){
        std::cout << "Consumer() called" << std::endl;
    }
    // Called when AST for TU is ready/has been parsed
    void HandleTranslationUnit(clang::ASTContext& Context){
        std::cout << "HandleTU() called" << std::endl;

        CyclomaticComplexityAnalysis CCA(Context);
        CCA.run();

        UsingAnalysis UA(Context);
        UA.run();

        LoopDepthAnalysis LDA(Context, 4);
        LDA.run();

        LoopKindAnalysis LKA(Context);
        LKA.run();
    }
};


// Responsible for steering when what is executed
class Action : public ASTFrontendAction {
public:
    Action(){
        std::cout << "Creating AST action" << std::endl;
    }
    // Called after frontend is initialized, but before per-file processing
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        CompilerInstance &CI, llvm::StringRef InFile){
            std::cout << "CreateASTConsumer() called" << std::endl;
            return std::make_unique<Consumer>();
    }
    // Called at start of processing a single input
    bool BeginSourceFileAction(CompilerInstance& CI) {
        std::cout << "Starting to process file" << std::endl;
        return true;
    }
};


using namespace clang::tooling;
// Options in CLI specific/nongeneric to clang-stat
llvm::cl::OptionCategory ClangStatCategory("clang-stat options");
llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
llvm::cl::extrahelp MoreHelp("\nMore help text coming soon...\n");
// CL options
llvm::cl::opt<bool> FSOption(
    "forstmt",
    llvm::cl::desc("Whether we want to catch for statements "),
    llvm::cl::cat(ClangStatCategory)
);

// Responsible for building Actions
class Factory : public clang::tooling::FrontendActionFactory {
public:
    // 'ctor'
    std::unique_ptr<FrontendAction> create() override {
        return std::make_unique<Action>();
    }
};


int main(int argc, const char** argv){

    // parses all options that command-line tools have in common
    CommonOptionsParser Parser(argc, argv, ClangStatCategory);

    ClangTool Tool(Parser.getCompilations(), Parser.getSourcePathList());

    return Tool.run(new Factory()); // input file, .cpp or .ast
}
