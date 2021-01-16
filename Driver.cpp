// helpful: https://github.com/peter-can-talk/cppnow-2017/blob/master/code/mccabe/mccabe.cpp

// clang includes
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"
#include "clang/Basic/FileManager.h"
#include "llvm/Support/Path.h"

// standard includes
#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>

#include "cxx-langstat/AnalysisRegistry.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;


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
        ordered_json AllAnalysesFeatures;
        // Run enabled analyses and get features
        int i=0;
        for(const auto& an : Registry->Analyses){
            an->run(InFile, Context);
            auto AnalysisAbbreviation = Registry
                ->Options.EnabledAnalyses.Items[i].Name.str();
            AllAnalysesFeatures[AnalysisAbbreviation]=an->getResult();
            i++;
        }
        // Output to output dir/filename.cpp.json
        auto OutputFile = Registry->Options.OutDirectory
            + llvm::sys::path::filename(InFile).str() + ".json";
        std::ofstream o(OutputFile);
        o << AllAnalysesFeatures.dump(4) << '\n';

        std::cout << AllAnalysesFeatures.dump(4) << std::endl;
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

// Responsible for building Actions
class Factory : public clang::tooling::FrontendActionFactory {
public:
    // ctor
    Factory(AnalysisRegistry* Reg) : Registry(Reg){
    }
    //
    std::unique_ptr<FrontendAction> create() override {
        return std::make_unique<Action>(Registry);
    }
    AnalysisRegistry* Registry;
};

//-----------------------------------------------------------------------------
// Global variables
using namespace clang::tooling;
// Options in CLI specific to cxx-langstat
llvm::cl::OptionCategory CXXLangstatCategory("cxx-langstat options", "description");
llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
// llvm::cl::extrahelp MoreHelp("\nMore help text coming soon...\n");
// CL options
// Accepts comma-separated string of analyses
llvm::cl::opt<std::string> AnalysesOption(
    "analyses",
    llvm::cl::desc("Comma-separated list of analyses"),
    llvm::cl::cat(CXXLangstatCategory));
// Path of directory where to store JSON files containing features,
// path can be relative or absolute
llvm::cl::opt<std::string> OutDirectoryOption(
    "out",
    llvm::cl::desc("Directory where out"),
    llvm::cl::Required,
    llvm::cl::ValueRequired,
    llvm::cl::cat(CXXLangstatCategory));

//-----------------------------------------------------------------------------

int main(int argc, const char** argv){
    // Parser for command line options, provided by llvm
    CommonOptionsParser Parser(argc, argv, CXXLangstatCategory);
    CXXLangstatOptions Opts(AnalysesOption, OutDirectoryOption);
    AnalysisRegistry* Registry = new AnalysisRegistry(Opts);
    std::cout << "rel " << OutDirectoryOption << std::endl;
    std::cout << "abs " << getAbsolutePath(llvm::StringRef(OutDirectoryOption)) << std::endl;

    const std::vector<std::string>& spl = Parser.getSourcePathList();
    CompilationDatabase& db = Parser.getCompilations();
    ClangTool Tool(db, spl);
    for (auto s : Parser.getSourcePathList()){
        std::cout << s << '\n';
    }
    // Tool is run for every file specified in source path list
    Tool.run(std::make_unique<Factory>(Registry).get()); // input file, .cpp or .ast
    // Not really important here, but good practice
    delete Registry;
    std::cout << "Reached end of program" << std::endl;
    return 0;
}
