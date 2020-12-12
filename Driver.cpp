// helpful: https://github.com/peter-can-talk/cppnow-2017/blob/master/code/mccabe/mccabe.cpp

// clang includes
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"

// standard includes
#include <iostream>

//
// #include "cxx-langstat/Analyses/CyclomaticComplexityAnalysis.h"
#include "cxx-langstat/Analyses/LoopDepthAnalysis.h"
// #include "cxx-langstat/Analyses/LoopKindAnalysis.h"

// #include "cxx-langstat/Analyses/StdlibAnalysis.h"
// #include "cxx-langstat/Analyses/StdlibAnalysis2.h"

// #include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"
// #include "cxx-langstat/Analyses/TemplateParameterAnalysis.h"
// #include "cxx-langstat/Analyses/UsingAnalysis.h"
// #include "cxx-langstat/Analyses/VariableTemplateAnalysis.h"

#include "cxx-langstat/AnalysisRegistry.h"

using namespace clang;
using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------

// Consumes the AST, i.e. does computations on it
class Consumer : public ASTConsumer {
public:
    Consumer(llvm::StringRef InFile, AnalysisRegistry* Registry2) : InFile(InFile), Registry2(Registry2){
    }
    // Called when AST for TU is ready/has been parsed
    void HandleTranslationUnit(clang::ASTContext& Context){
        std::cout << "Handling the translation unit" << std::endl;

        // fine
        // AnalysisRegistry RA("lda");
        // RA.runEnabledAnalyses(InFile, Context);
        // segfault
        // Registry.runEnabledAnalyses(InFile, Context);
        // fine
        // Registry.LDA.run(InFile, Context);

        // fine
        // Registry.Analyses[0]->run(InFile, Context);
        // Registry.AnalysisMapping["lka"]->run(InFile, Context);

        // Registry2->AnalysisMapping["lda"]->run(InFile, Context);

        // Registry2->runEnabledAnalyses(InFile, Context);

        // for(auto a : Registry2->EnabledAnalyses.Items)
            // Registry2->AnalysisMapping[a.Name.str()]->run(InFile, Context);



        // fine
        // Registry2->Analyses[0]->run(InFile, Context);


        int i=0;
        for(auto ab : Registry2->Abbrev) {
            if(Registry2->EnabledAnalyses.contains(ab)){
                Registry2->Analyses[i]->run(InFile, Context);
            }
            i++;
        }



    }
public:
    llvm::StringRef InFile;
    static AnalysisRegistry Registry;
    AnalysisRegistry* Registry2;
};

// Responsible for steering when what is executed
class Action : public ASTFrontendAction {
public:
    Action(AnalysisRegistry* Registry2) : Registry2(Registry2){
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
            return std::make_unique<Consumer>(getCurrentFile(), Registry2);
    }
    //
    void EndSourceFileAction(){
        std::cout
        << "Finished processing " << getCurrentFile().str() << ".\n";
    }
    AnalysisRegistry* Registry2;
};

AnalysisRegistry* Registry2 = new AnalysisRegistry();

// Responsible for building Actions
class Factory : public clang::tooling::FrontendActionFactory {
public:
    // 'ctor'
    std::unique_ptr<FrontendAction> create() override {
        return std::make_unique<Action>(Registry2);
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

std::string t;
// set registry for ast consumer, use loop depth analysis only
AnalysisRegistry Consumer::Registry = AnalysisRegistry(t);

int main(int argc, const char** argv){

    // parses all options that command-line tools have in common
    CommonOptionsParser Parser(argc, argv, CXXLangstatCategory);

    Registry2->setEnabledAnalyses(AnalysesOption);

    ClangTool Tool(Parser.getCompilations(), Parser.getSourcePathList());
    // Tool is run for every file specified in source path list
    return Tool.run(std::make_unique<Factory>().get()); // input file, .cpp or .ast
}
