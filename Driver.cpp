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
        Stage Stage = Registry->Options.Stage;
        // analyze code
        if(Stage == 0 || Stage == 1){
            // Run enabled analyses and get features
            int i=0;
            for(const auto& an : Registry->Analyses){ // ref to unique_ptr bad?
                an->run(InFile, Context);
                auto AnalysisAbbreviation = Registry
                    ->Options.EnabledAnalyses.Items[i].Name.str();
                AllAnalysesFeatures[AnalysisAbbreviation]=an->getResult();
                i++;
            }
            // Output to output dir/filename.cpp.json
            llvm::StringRef filename = llvm::sys::path::filename(InFile);
            filename.consume_back(llvm::sys::path::extension(filename));
            auto OutputFile = Registry->Options.OutputFiles[FileIndex]
                + filename.str() + ".features.json";
            std::ofstream o(OutputFile);
            o << AllAnalysesFeatures.dump(4) << '\n';
            FileIndex++;
            std::cout << AllAnalysesFeatures.dump(4) << std::endl;
        }

        // process features from json (not from disk)
        if(Stage == 0){
            for(const auto& an : Registry->Analyses){ // ref to unique_ptr bad?
                an->processJSON(AllAnalysesFeatures);
            }
        }
    }
public:
    llvm::StringRef InFile;
    AnalysisRegistry* Registry;
    static unsigned FileIndex;
};
unsigned Consumer::FileIndex=0;

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
llvm::cl::OptionCategory CXXLangstatCategory("cxx-langstat options", "");
llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
// llvm::cl::extrahelp MoreHelp("\nMore help text coming soon...\n");
// CL options

// # probably going to retire soon
// Accepts comma-separated string of analyses
llvm::cl::opt<std::string> AnalysesOption(
    "analyses",
    llvm::cl::desc("Comma-separated list of analyses"),
    llvm::cl::cat(CXXLangstatCategory));

// #legacy
// Path of directory where to store JSON files containing features,
// path can be relative or absolute
// llvm::cl::opt<std::string> OutDirectoryOption(
//     "out_old",
//     llvm::cl::desc("Directory where out"),
//     llvm::cl::Required,
//     llvm::cl::ValueRequired,
//     llvm::cl::cat(CXXLangstatCategory));
// llvm::cl::opt<bool> ProcessOption(
//     "process_",
//     llvm::cl::desc("Flag to process features into statistics"),
//     llvm::cl::cat(CXXLangstatCategory));

// 2 flags:
// --emit-features: analysis stops after writing features to file after reading in .ast
// --emit-statistics: read in JSON with features and compute statistics
// no flag at all: emit JSON with both features and statistics
llvm::cl::opt<Stage> PipelineStage(
    llvm::cl::desc("stages"),
    llvm::cl::values(
        clEnumValN(emit_features, "emit-features", "Stop after emitting features"),
        clEnumValN(emit_statistics, "emit-statistics", "Read in JSON file of features and compute statistics")),
    llvm::cl::cat(CXXLangstatCategory));


// new means to give analyses, still inactive
llvm::cl::list<AnalysisType> AnalysesList (
    llvm::cl::desc("available analyses"),
    llvm::cl::values(
        clEnumVal(cca, "cyclo")
    ),
    llvm::cl::cat(CXXLangstatCategory));
llvm::cl::alias CCAAlias("cyclo", llvm::cl::desc(" a"), llvm::cl::aliasopt(AnalysesList));

// --in option, all until --out is taken as input files
llvm::cl::list<std::string> InputFilesOption(
    "in",
    llvm::cl::Positional,
    // llvm::cl::Prefix,
    // llvm::cl::PositionalEatsArgs,
    llvm::cl::desc("<source0> [... <sourceN>]"),
    // llvm::cl::ValueOptional,
    llvm::cl::ZeroOrMore,
    llvm::cl::cat(CXXLangstatCategory));

// https://clang.llvm.org/doxygen/CommonOptionsParser_8h_source.html @ line 91
    // static cl::list<std::string> SourcePaths(
   // 92       cl::Positional, cl::desc("<source0> [... <sourceN>]"), OccurrencesFlag,
   // 93       cl::cat(Category), cl::sub(*cl::AllSubCommands));

// --out option, optional. when used, should give same #args as with --in.
llvm::cl::list<std::string> OutputFilesOption(
    "out",
    llvm::cl::Positional,
    // llvm::cl::Prefix,
    // llvm::cl::PositionalEatsArgs,
    llvm::cl::desc("<Output files>"),
    // llvm::cl::ValueOptional,
    llvm::cl::ZeroOrMore,
    llvm::cl::cat(CXXLangstatCategory));


//-----------------------------------------------------------------------------

int main(int argc, const char** argv){
    // Parser for command line options, provided by llvm
    // I do not like the way input/source files are given by COP, so I roll
    // my own stuff.
    // CommonOptionsParser Parser(argc, argv, CXXLangstatCategory);
    llvm::cl::ParseCommandLineOptions(argc, argv);
    // Create custom options object
    CXXLangstatOptions Opts(PipelineStage, OutputFilesOption, AnalysesOption);

    // llvm::StringMap<llvm::cl::Option*> &Map = llvm::cl::getRegisteredOptions();

    std::cout << "stage " << Opts.Stage << std::endl;
    std::cout << "output\n";
    std::cout << "o size " << OutputFilesOption.size() << std::endl;
    for (auto s : Opts.OutputFiles)
        std::cout << s << std::endl;
    std::cout << "intput\n";
    // const std::vector<std::string>& spl = Parser.getSourcePathList();
    const std::vector<std::string>& spl = InputFilesOption;
    for (auto s : spl){
        std::cout << s << '\n';
    }

    // Either no output files are specified -> store at working directory
    // Or ensure that for each input and output file is specified.
    if(OutputFilesOption.size()>0 && OutputFilesOption.size()!=spl.size()){
        std::cout << "not the right amount of output files specified, quitting..\n";
        exit(1);
    } else if(OutputFilesOption.size()==0){
        for(const auto& : spl)
            Opts.OutputFiles.emplace_back()
    }

    AnalysisRegistry* Registry = new AnalysisRegistry(Opts);
    // std::cout << "rel " << OutDirectoryOption << std::endl;
    // std::cout << "abs " << getAbsolutePath(llvm::StringRef(OutDirectoryOption)) << std::endl;

    if(Registry->Options.Stage != 2){
        // CompilationDatabase& db = Parser.getCompilations();
        // https://clang.llvm.org/doxygen/CommonOptionsParser_8h_source.html @ line 109
        std::string ErrorMessage;
        std::unique_ptr<CompilationDatabase> db =
            FixedCompilationDatabase::loadFromCommandLine(argc, argv, ErrorMessage);
        ClangTool Tool(*db, spl);
        // Tool is run for every file specified in source path list
        Tool.run(std::make_unique<Factory>(Registry).get()); // input file, .cpp or .ast
    }

    // process features to statistics from disk
    else if(Registry->Options.Stage == 2){
        for(auto File : spl){
            ordered_json FromFile;
            std::ifstream i(File);
            i >> FromFile;
            for(const auto& an : Registry->Analyses){ // ref to unique_ptr bad?
                an->processJSON(FromFile);
            }
        }
    }

    // Not really important here, but good practice
    delete Registry;
    std::cout << "Reached end of program" << std::endl;
    return 0;
}
