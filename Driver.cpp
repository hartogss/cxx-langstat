// helpful: https://github.com/peter-can-talk/cppnow-2017/blob/master/code/mccabe/mccabe.cpp

// clang includes
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Path.h"
// standard includes
#include <iostream> // should be removed
#include <fstream> // file stream
// JSON library
#include <nlohmann/json.hpp>
//
#include "cxx-langstat/AnalysisRegistry.h"
#include "cxx-langstat/Utils.h"
#include "cxx-langstat/Options.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using StringRef = llvm::StringRef;
using ASTContext = clang::ASTContext;
using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------
// Consumes the AST, i.e. does computations on it
class Consumer : public ASTConsumer {
public:
    Consumer(StringRef InFile, AnalysisRegistry* Registry) :
        InFile(InFile),
        Registry(Registry){
    }
    // Called when AST for TU is ready/has been parsed
    void HandleTranslationUnit(ASTContext& Context){
        std::cout << "Handling the translation unit" << std::endl;
        ordered_json AllAnalysesFeatures;
        Stage Stage = Registry->Options.Stage;
        int AnalysisIndex=0;
        for(const auto& an : Registry->Analyses){ // ref to unique_ptr bad?
            auto AnalysisAbbreviation = Registry
                ->Options.EnabledAnalyses.Items[AnalysisIndex].Name.str();
            //
            if(Stage != emit_statistics){
                // Analyze clang AST and extract features
                an->run(InFile, Context);
                AllAnalysesFeatures[AnalysisAbbreviation]=an->getFeatures();
            }
            // process features from json (not from disk)
            if(Stage == none){
                an->processFeatures(AllAnalysesFeatures[AnalysisAbbreviation]);
                /// FIXME: actually write statistics to disk
            }
            AnalysisIndex++;
        }
        // Write to file if -emit-features is active
        if(Stage == emit_features){
            auto OutputFile = Registry->Options.OutputFiles[FileIndex];
            std::ofstream o(OutputFile);
            o << AllAnalysesFeatures.dump(4) << '\n';
            FileIndex++;
        }
        std::cout << "all features:" << AllAnalysesFeatures.dump(4) << std::endl;
    }
public:
    StringRef InFile;
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
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(
        CompilerInstance &CI, StringRef InFile){
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

// 2 flags:
// --emit-features: analysis stops after writing features to file after reading in .ast
// --emit-statistics: read in JSON with features and compute statistics
// no flag at all: compute features but don't write them to disk, compute
// statistics and emit them
llvm::cl::opt<Stage> PipelineStage(
    llvm::cl::desc("Stage: "),
    llvm::cl::values(
        clEnumValN(emit_features, "emit-features",
            "Stop after emitting features.\n"
            "If output files are specified, \n "
            "on output for every input file \n"
            "has to be specified. If no output \n"
            "is specified, output files will \n"
            "will be put at the working directory"),
        clEnumValN(emit_statistics, "emit-statistics", "Read in .json files "
            "each containing features extracted from an AST and compute "
            "statistics. Requires either no output files or only a single one")),
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
    llvm::cl::desc("<ast0> [... <astN>]"),
    // llvm::cl::ValueOptional,
    llvm::cl::ZeroOrMore,
    llvm::cl::cat(CXXLangstatCategory));

// --out option, optional. when used, should give same #args as with --in.
llvm::cl::list<std::string> OutputFilesOption(
    "out",
    llvm::cl::Positional,
    llvm::cl::desc("[<json1> ... <jsonN>]"),
    // llvm::cl::ValueOptional,
    llvm::cl::ZeroOrMore,
    llvm::cl::cat(CXXLangstatCategory));

// what to do with this? some -p option already there by default, but parser fails on it
static llvm::cl::opt<std::string> BuildPath("p", llvm::cl::desc("Build path, but really"),
   llvm::cl::Optional, llvm::cl::cat(CXXLangstatCategory));

//-----------------------------------------------------------------------------

int main(int argc, const char** argv){
    // Common parser for command line options, provided by llvm
    // CommonOptionsParser Parser(argc, argv, CXXLangstatCategory);
    // const std::vector<std::string>& spl = Parser.getSourcePathList();
    // CompilationDatabase& db = Parser.getCompilations();
    // I don't like the way input/source files are handled by COP, so I roll
    // my own stuff.
    std::unique_ptr<CompilationDatabase> db = nullptr;
    // First try to get string of cxxflags after '--', as loadFromCommandLine requires

    std::string ErrorMessage;
    db = FixedCompilationDatabase::loadFromCommandLine(argc, argv, ErrorMessage);
    if(db) {
        std::cout << "COMPILE COMMAND: ";
        for(auto cc : db->getCompileCommands("")){
            for(auto s : cc.CommandLine){
                std::cout << s << " ";
            }
            std::cout << std::endl;
        }
    } else {
        std::cout << "Could not load compile command from command line \n" +
            ErrorMessage;
    }

    // Only now can call this method, otherwise compile command could be interpreted
    // as input or output file since those are positional
    // This usage is encouraged this way according to
    // https://clang.llvm.org/doxygen/classclang_1_1tooling_1_1FixedCompilationDatabase.html#a1443b7812e6ffb5ea499c0e880de75fc
    llvm::cl::ParseCommandLineOptions(argc, argv, "cxx-langstat is a clang-based"
     "tool for computing statistics on C/C++ code on the clang AST level");
    const std::vector<std::string>& spl = InputFilesOption;
    std::vector<std::string>& OutputFiles = OutputFilesOption;

    std::cout << "input files(" << spl.size() << "): ";
    for(const auto& InputFile : spl){
        std::cout << InputFile << " ";
        if(StringRef(InputFile).consume_back("/")){
            std::cout << "Specified input dir, quitting.. \n";
            exit(1);
        }
    }
    std::cout << '\n';
    std::cout << "output files(" << OutputFiles.size() << "): ";
    for(const auto& OutputFile : OutputFiles){
        std::cout << OutputFile << " ";
        if(StringRef(OutputFile).consume_back("/")){
            std::cout << "Specified output dir, quitting.. \n";
            exit(1);
        }
    }
    std::cout << '\n';
    if(PipelineStage == emit_features){
        // No output files specified -> store at working dir
        // If specified, check that #input files = #output files
        if(OutputFiles.size()==0){
            for(const auto& InputFile : spl){
                StringRef filename = llvm::sys::path::filename(InputFile);
                filename.consume_back(llvm::sys::path::extension(filename)); // use replace_extension
                OutputFiles.emplace_back("./" + filename.str() + ".features.json");
            }
        } else if(OutputFiles.size()>0 && OutputFiles.size()!=spl.size()){
            std::cout << "#Source files != #Output files, quitting..\n";
            exit(1);
        }
    // When -emit-features option is not used, zero or one output file is ok.
    } else {
        if(OutputFiles.size()==0){
            OutputFiles.emplace_back("./stats.json");
        }
        if(OutputFiles.size()>1){
            std::cout << "Can only specify multiple output files with -emit-features, quitting..\n";
            exit(1);
        }
    }

    // Create custom options object for registry
    CXXLangstatOptions Opts(PipelineStage, OutputFiles, AnalysesOption);
    AnalysisRegistry* Registry = new AnalysisRegistry(Opts);

    if(PipelineStage != emit_statistics){
        // https://clang.llvm.org/doxygen/CommonOptionsParser_8cpp_source.html @ 109
        // Read in database found in dir specified by -p or a parent path
        std::string ErrorMessage;
        if(!BuildPath.empty()){
            std::cout << "READING BUILD PATH\n";
            std::cout << "BUILD PATH: " << BuildPath << std::endl;
            db = CompilationDatabase::autoDetectFromDirectory(BuildPath, ErrorMessage);
            if(!ErrorMessage.empty()){
                std::cout << ErrorMessage << std::endl;
                exit(1);
            }
            std::cout << "FOUND COMPILE COMMANDS:" << std::endl;
            for(auto cc : db->getAllCompileCommands()){
                for(auto s : cc.CommandLine){
                    std::cout << s << " ";
                }
                std::cout << std::endl;
            }
        }
        if(db){
            ClangTool Tool(*db, spl);
            // Tool is run for every file specified in source path list
            Tool.run(std::make_unique<Factory>(Registry).get());
        } else {
            std::cout << "The tool couldn't run due to missing compilation database "
                "Please try one of the following:\n"
                " When analyzing .ast files, append \"--\", to indicate no CDB is necessary\n"
                " When analyzing .cpp files, append \"-- <compile flags>\" or \"-p <build path>\"\n";
        }
    }

    // Process features stored on disk to statistics
    else if(PipelineStage == emit_statistics){
        std::cout << "do because stage 2" << std::endl;
        ordered_json AllFilesAllStatistics;
        for(auto File : spl){
            ordered_json j;
            std::ifstream i(File);
            i >> j;
            auto AnalysisIndex = 0;
            ordered_json OneFileAllStatistics;
            for(const auto& an : Registry->Analyses){ // ref to unique_ptr bad?
                auto AnalysisAbbreviation = Registry
                    ->Options.EnabledAnalyses.Items[AnalysisIndex].Name.str();
                an->processFeatures(j[AnalysisAbbreviation]);
                for(const auto& [statdesc, stats] : an->getStatistics().items()){
                    OneFileAllStatistics[statdesc] = stats;
                }
                AnalysisIndex++;
            }
            AllFilesAllStatistics[File] = OneFileAllStatistics;
        }
        std::ofstream o(Registry->Options.OutputFiles[0]);
        o << AllFilesAllStatistics.dump(4) << std::endl;
        std::cout << "all stats\n" << AllFilesAllStatistics.dump(4) << std::endl;
    }

    // Not really important here, but good practice
    delete Registry;
    std::cout << "Reached end of program" << std::endl;
    return 0;
}
