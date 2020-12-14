// helpful: https://github.com/peter-can-talk/cppnow-2017/blob/master/code/mccabe/mccabe.cpp

// clang includes
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"

// standard includes
#include <iostream>

#include "cxx-langstat/AnalysisRegistry.h"

using namespace clang;
using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------

// Consumes the AST, i.e. does computations on it
class Consumer : public ASTConsumer {
public:
    Consumer(llvm::StringRef InFile, AnalysisRegistry* Registry) :
        InFile(InFile),
        Registry(Registry){
    }
    // Called when AST for TU is ready/has been parsed
    void HandleTranslationUnit(clang::ASTContext& Context){
        std::cout << "Handling the translation unit" << std::endl;

        int i=0;
        for(auto ab : Registry->Abbrev) {
            if(Registry->EnabledAnalyses.contains(ab)){
                Registry->Analyses[i]->run(InFile, Context);
            }
            i++;
        }
    }
public:
    llvm::StringRef InFile;
    AnalysisRegistry* Registry;
};

// Responsible for steering when what is executed
class Action : public ASTFrontendAction {
public:
    Action(AnalysisRegistry* Registry) : Registry(Registry){
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
            return std::make_unique<Consumer>(getCurrentFile(), Registry);
    }
    //
    void EndSourceFileAction(){
        std::cout
        << "Finished processing " << getCurrentFile().str() << ".\n";
    }
    AnalysisRegistry* Registry;
};

//-----------------------------------------------------------------------------
// Might be more elegant way, but currently only way I know to get Factory to
// pass registry to Action
AnalysisRegistry* Registry = new AnalysisRegistry();
//-----------------------------------------------------------------------------

// Responsible for building Actions
class Factory : public clang::tooling::FrontendActionFactory {
public:
    // 'ctor'
    std::unique_ptr<FrontendAction> create() override {
        return std::make_unique<Action>(Registry);
    }
};

//-----------------------------------------------------------------------------
// Global variables
using namespace clang::tooling;
// Options in CLI specific to cxx-langstat
llvm::cl::OptionCategory CXXLangstatCategory("cxx-langstat options", "description");
llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
// llvm::cl::extrahelp MoreHelp("\nMore help text coming soon...\n");
// CL options
llvm::cl::opt<std::string> AnalysesOption(
    "analyses",
    llvm::cl::desc("Comma-separated list of analyses"),
    llvm::cl::cat(CXXLangstatCategory)
);

//-----------------------------------------------------------------------------

int main(int argc, const char** argv){

    // parses all options that command-line tools have in common
    CommonOptionsParser Parser(argc, argv, CXXLangstatCategory);

    Registry->setEnabledAnalyses(AnalysesOption);

    ClangTool Tool(Parser.getCompilations(), Parser.getSourcePathList());
    // Tool is run for every file specified in source path list
    Tool.run(std::make_unique<Factory>().get()); // input file, .cpp or .ast
    // Not really important here, but good practice
    delete Registry;
    return 0;
}
