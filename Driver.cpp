// helpful: https://github.com/peter-can-talk/cppnow-2017/blob/master/code/mccabe/mccabe.cpp

// clang includes
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"

// standard includes
#include <iostream>

//
#include "cxx-langstat/Analyses/CyclomaticComplexityAnalysis.h"
#include "cxx-langstat/Analyses/LoopDepthAnalysis.h"
#include "cxx-langstat/Analyses/LoopKindAnalysis.h"
#include "cxx-langstat/Analyses/StdlibAnalysis.h"
#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"
#include "cxx-langstat/Analyses/TemplateParameterAnalysis.h"
#include "cxx-langstat/Analyses/UsingAnalysis.h"
#include "cxx-langstat/Analyses/VariableTemplateAnalysis.h"

using namespace clang;
using namespace clang::ast_matchers;


// Consumes the AST, i.e. does computations on it
class Consumer : public ASTConsumer {
public:
    Consumer(){
    }
    // Called when AST for TU is ready/has been parsed
    void HandleTranslationUnit(clang::ASTContext& Context){
        std::cout << "Handling the translation unit" << std::endl;
        CyclomaticComplexityAnalysis CCA(Context);
        CCA.run();
        LoopDepthAnalysis LDA(Context, 4);
        LDA.run();
        LoopKindAnalysis LKA(Context);
        LKA.run();
        StdlibAnalysis SLA(Context);
        SLA.run();
        TemplateInstantiationAnalysis TIA(Context);
        TIA.run();
        TemplateParameterAnalysis TPA(Context);
        TPA.run();
        UsingAnalysis UA(Context);
        UA.run();
        VariableTemplateAnalysis VTA(Context);
        VTA.run();
    }
};


// Responsible for steering when what is executed
class Action : public ASTFrontendAction {
public:
    Action(){
        std::cout << "Creating AST Action" << std::endl;
    }
    // Called at start of processing a single input
    bool BeginSourceFileAction(CompilerInstance& CI) {
        std::cout
        << "Starting to process " << getCurrentFile().str()
        << ". AST=" << isCurrentFileAST() << ".\n";
        return true;
    }
    // Called after frontend is initialized, but before per-file processing
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        CompilerInstance &CI, llvm::StringRef InFile){
            std::cout << "Creating AST Consumer" << std::endl;
            return std::make_unique<Consumer>();
    }
    //
    void EndSourceFileAction(){
        std::cout
        << "Finished processing " << getCurrentFile().str() << ".\n";
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

    return Tool.run(std::make_unique<Factory>().get()); // input file, .cpp or .ast
}
