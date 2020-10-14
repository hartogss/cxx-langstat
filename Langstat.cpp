// helpful: https://github.com/peter-can-talk/cppnow-2017/blob/master/code/mccabe/mccabe.cpp

#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

//plugin support
#include "clang/Frontend/FrontendPluginRegistry.h"

#include <iostream>

using namespace clang; // CompilerInstance, ASTFrontendAction, ASTConsumer

// Class called on a match
class MatchHandler : public ast_matchers::MatchFinder::MatchCallback {
    // Gets called on a match by MatchFinder
    void run(const ast_matchers::MatchFinder::MatchResult& Result){
        std::cout << "Found match in AST" << std::endl;
        auto ForStmtNode = Result.Nodes.getNodeAs<ForStmt>("fs");
        ForStmtNode->dump();
        auto WhileStmtNode = Result.Nodes.getNodeAs<WhileStmt>("ws");
        WhileStmtNode->dump();
    }
};


// Consumes, i.e. does computations on the AST
class Consumer : public ASTConsumer {
public:
    // Constructor, how is finder & handler constructed?
    Consumer(bool forstmtflag, bool whilestmtflag){
        std::cout << "Consumer() called" << std::endl;
        using namespace clang::ast_matchers; //what does 'using' without 'namespace' do? -> sort of typedef
        StatementMatcher ForLoopStmt = forStmt().bind("fs");
        ForLoopStmt = forStmt(unless(hasDescendant(forStmt())), unless(hasAncestor(forStmt()))).bind("fs");
        StatementMatcher WhileLoopStmt = whileStmt().bind("ws");

        if(forstmtflag){
            std::cout << "Adding matcher to finder" << std::endl;
            Finder.addMatcher(ForLoopStmt, &Handler);
        }
        if(whilestmtflag){
            std::cout << "Adding matcher to finder" << std::endl;
            Finder.addMatcher(WhileLoopStmt, &Handler);
        }
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
    Action(){
        std::cout << "Creating AST action" << std::endl;
    }
    Action(bool forstmtflag, bool whilestmtflag) : forstmtflag(forstmtflag), whilestmtflag(whilestmtflag){
        std::cout << "Creating AST action with arg" << std::endl;
        forstmtflag = 0;
    }
    // Called after frontend is initialized, but before per-file processing
    // why virtual?
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        CompilerInstance &CI, llvm::StringRef InFile){
            std::cout << "CreateASTConsumer() called" << std::endl;
            std::cout << forstmtflag << std::endl;
            return std::make_unique<Consumer>(forstmtflag, whilestmtflag);
    }
    // Called at start of processing a single input
    bool BeginSourceFileAction(CompilerInstance& CI) {
        std::cout << "Starting to process file" << std::endl;
        return true;
    }
private:
    bool forstmtflag;
    bool whilestmtflag;
};

class Analysis {
    using StatementMatcher = clang::ast_matchers::StatementMatcher;
public:
    Analysis(StatementMatcher Matcher) : Matcher(Matcher){
    }
private:
    std::string name;
    // should become more general for example to support analyses that rely on
    // something else than matchers -> maybe with inheritance? ;)
    StatementMatcher Matcher;
    // Runner: defines what happens when matchcallback is called on a match
    // std::function<>
};

class Analyses {
private:
    std::vector<Analysis> an;
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
class Factory : public clang::tooling::FrontendActionFactory {
public:
    std::unique_ptr<FrontendAction> create() override {
        return std::make_unique<Action>(FSOption, WSOption);
    }
};


// const: pass in this case by pointer (could be reference too), but
// restrict callee from being able to change passed data
// avoids (maybe slow) copy
int main(int argc, const char** argv){
    using namespace clang::tooling;

    // parses all options that command-line tools have in common
    CommonOptionsParser Parser(argc, argv, ClangStatCategory);

    ClangTool Tool(Parser.getCompilations(),
                 Parser.getSourcePathList());

    // util to run frontendaction over file
    // ClangTool CT
    // clang::tooling::runToolOnCode(std::make_unique<Action>(), argv[1]); //uses string of code from command line
    return Tool.run(new Factory()); // input file, .cpp or .ast
    // return 0;
}
