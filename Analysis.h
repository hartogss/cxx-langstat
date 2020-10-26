#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"


//-----------------------------------------------------------------------------

// Class called on a match
class MatchHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
    // Gets called on a match by MatchFinder
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& Result);
};

//-----------------------------------------------------------------------------


// Consumes the AST, i.e. does computations on the AST
// class Consumer : public clang::ASTConsumer {
// public:
//     // ctor
//     Consumer();
//     // Called when AST for TU is ready/has been parsed
    // virtual void HandleTranslationUnit(clang::ASTContext& Context)=0;
// private:
//     // Brings together all Matchers and callbacks when something matches
    // clang::ast_matchers::MatchFinder Finder;
    // MatchHandler Handler;
//     // how are these contructed?
// };

//-----------------------------------------------------------------------------

// Responsible for steering when what is executed
// class Action : public clang::ASTFrontendAction {
// public:
    // // Called at start of processing a single input
    // virtual bool BeginSourceFileAction(clang::CompilerInstance& CI) override;
    // Called after frontend is initialized, but before per-file processing
    // virtual std::unique_ptr<clang::ASTConsumer>
    // CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile) override;
// };

//-----------------------------------------------------------------------------

// Responsible for building Actions to give ToolAction to ClangTool
// class Factory : public clang::tooling::FrontendActionFactory {
// public:
//     // 'ctor'
//     std::unique_ptr<clang::FrontendAction> create();
// };


#endif /* ANALYSIS_H */
