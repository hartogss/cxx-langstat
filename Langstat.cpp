// llvm & clang includes
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
// standard includes
#include <iostream>
// custom includes
#include "Analysis.h"

// namespaces
using namespace clang; // CompilerInstance, ASTFrontendAction, ASTConsumer
using namespace clang::ast_matchers; // StatementMatcher, actual AST matchers like forStmt etc.
using namespace clang::tooling; // CommonOptionsParser


//-----------------------------------------------------------------------------

// Class called on a match
class MatchHandler : public ast_matchers::MatchFinder::MatchCallback {
public:
    // ctor
    MatchHandler() = default;
    // Gets called on a match by MatchFinder
    void run(const ast_matchers::MatchFinder::MatchResult& Result){
        std::cout << "Found match in AST" << std::endl;
        // is everyting Stmt*? I thought no general class for stmt,decl,expr
        const clang::Stmt* node = Result.Nodes.getNodeAs<clang::Stmt>(An.Name);
        An.Runner(node);
    }
    Analysis An;
};

//-----------------------------------------------------------------------------


// Consumes the AST, i.e. does computations on the AST
class Consumer : public ASTConsumer {
public:
    // ctor
    Consumer(Analysis An){
        std::cout << "Consumer() called" << std::endl;
        std::cout << "Adding matchers to finder" << std::endl;
        Handler.An = An;
        Finder.addMatcher(An.getMatcher(), &Handler);
    }
    // Called when AST for TU is ready/has been parsed
    void HandleTranslationUnit(clang::ASTContext& Context){
        std::cout << "HandleTU() called" << std::endl;
        Finder.matchAST(Context);
    }
private:
    // Brings together all Matchers and callbacks when something matches
    ast_matchers::MatchFinder Finder;
    MatchHandler Handler;
    // how are these contructed?
};

//-----------------------------------------------------------------------------


// Responsible for steering when what is executed
class Action : public ASTFrontendAction {
public:
    // Constructor
    Action(){
        std::cout << "Creating AST action" << std::endl;
    }
    // ctor
    Action(Analysis An) : An(An){
        std::cout << "Creating AST action with arg" << std::endl;
    }
    // Called at start of processing a single input
    bool BeginSourceFileAction(CompilerInstance& CI) {
        std::cout << "Starting to process file" << std::endl;
        std::cout << getCurrentFile().data() << std::endl;
        return true;
    }
    // Called after frontend is initialized, but before per-file processing
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        CompilerInstance &CI, llvm::StringRef InFile){
            std::cout << "CreateASTConsumer() called" << std::endl;
            return std::make_unique<Consumer>(An);
    }
private:
    Analysis An;
};

//-----------------------------------------------------------------------------
// CL options

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
llvm::cl::opt<bool> WSOption(
    "whilestmt",
    llvm::cl::desc("Whether we want to catch while statements "),
    llvm::cl::cat(ClangStatCategory)
);

//-----------------------------------------------------------------------------
// Should move to separate file

// Add analysis
std::string Name = "fs";
StatementMatcher Matcher = forStmt().bind(Name);
auto Runner = [](auto Node) {
    std::cout << "fs matcher runner called" << std::endl;
    Node->dump();
};
Analysis forstmt(Name, Matcher, Runner);

//-----------------------------------------------------------------------------

// Responsible for building Actions to give ToolAction to ClangTool
class Factory : public clang::tooling::FrontendActionFactory {
public:
    // 'ctor'
    std::unique_ptr<FrontendAction> create() {
        std::cout << "create() called" << std::endl;
        return std::make_unique<Action>(forstmt);
    }
};

//-----------------------------------------------------------------------------

int main(int argc, const char** argv){

    // parses all options that command-line tools have in common
    CommonOptionsParser Parser(argc, argv, ClangStatCategory);

    // Print all file name & locations
    for (auto SourceFilePath : Parser.getSourcePathList()){
        std::cout << SourceFilePath << std::endl;
    }

    ClangTool Tool(Parser.getCompilations(), Parser.getSourcePathList());

    // Runs on string of code specified in command line
    // clang::tooling::runToolOnCode(std::make_unique<Action>(forstmt), argv[1]);
    // Runs over all files specified in command line
    Tool.run(new Factory());
    return 0;
}
