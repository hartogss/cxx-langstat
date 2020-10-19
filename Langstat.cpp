// helpful: https://github.com/peter-can-talk/cppnow-2017/blob/master/code/mccabe/mccabe.cpp

// clang includes
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

// standard includes
#include <iostream>

//
#include "Analysis.h"

//
using namespace clang; // CompilerInstance, ASTFrontendAction, ASTConsumer
using namespace clang::ast_matchers;


// Class called on a match
class MatchHandler : public ast_matchers::MatchFinder::MatchCallback {
public:
    MatchHandler () {
        Analysis an;
        An = an;
    }
    // Gets called on a match by MatchFinder
    void run(const ast_matchers::MatchFinder::MatchResult& Result){
        std::cout << "Found match in AST" << std::endl;
        auto node = Result.Nodes.getNodeAs<ForStmt>("fs");
        An.Runner();
    }
    Analysis An;
};


// Consumes, i.e. does computations on the AST
class Consumer : public ASTConsumer {
public:
    // Constructor, how is finder & handler constructed?
    Consumer(Analysis An){
        std::cout << "Consumer() called" << std::endl;
        using namespace clang::ast_matchers; //what does 'using' without 'namespace' do? -> sort of typedef
        // if(forstmtflag){
            std::cout << "Adding matchers to finder" << std::endl;
            Handler.An = An;
            Finder.addMatcher(An.getMatcher(), &Handler);
        // }
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
};


// Responsible for steering when what is executed, LibTooling
class Action : public ASTFrontendAction {
public:
    // Constructor
    // Action(){
    //     std::cout << "Creating AST action" << std::endl;
    // }
    Action(Analysis An) : An(An){
        std::cout << "Creating AST action with arg" << std::endl;
    }
    // Called after frontend is initialized, but before per-file processing
    // why virtual?
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        CompilerInstance &CI, llvm::StringRef InFile){
            std::cout << "CreateASTConsumer() called" << std::endl;
            return std::make_unique<Consumer>(An);
    }
    // Called at start of processing a single input
    bool BeginSourceFileAction(CompilerInstance& CI) {
        std::cout << "Starting to process file" << std::endl;
        return true;
    }
private:
    Analysis An;
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
llvm::cl::opt<bool> WSOption(
    "whilestmt",
    llvm::cl::desc("Whether we want to catch while statements "),
    llvm::cl::cat(ClangStatCategory)
);


// Responsible for building Actions
// class Factory : public clang::tooling::FrontendActionFactory {
// public:
//     // 'ctor'
//     std::unique_ptr<FrontendAction> create() override {
//         return std::make_unique<Action>();
//     }
// };


// const: pass in this case by pointer (could be reference too), but
// restrict callee from being able to change passed data
// avoids (maybe slow) copy
int main(int argc, const char** argv){
    using namespace clang::tooling;

    // parses all options that command-line tools have in common
    CommonOptionsParser Parser(argc, argv, ClangStatCategory);

    ClangTool Tool(Parser.getCompilations(),
                 Parser.getSourcePathList());

    using namespace clang::ast_matchers;
    StatementMatcher m = forStmt().bind("fs");
    std::function<void()> r = []() {std::cout << "worked" << std::endl;};
    Analysis forstmt(m, r);



    // util to run frontendaction over file
    // ClangTool CT
    clang::tooling::runToolOnCode(std::make_unique<Action>(forstmt), argv[1]); //uses string of code from command line
    // return Tool.run(new Factory()); // input file, .cpp or .ast
    return 0;
}
