#include "clang/Frontend/FrontendAction.h"
#include "Analysis.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"


#include <iostream>

using namespace clang; // CompilerInstance, ASTFrontendAction, ASTConsumer, ASTContext
using namespace clang::ast_matchers; // StatementMatcher, actual AST matchers like forStmt etc.
using namespace clang::tooling; // CommonOptionsParser

//-----------------------------------------------------------------------------

void MatchHandler::run(const MatchFinder::MatchResult& Result) {
    std::cout << "default run" << std::endl;
}

//-----------------------------------------------------------------------------

// void Consumer::HandleTranslationUnit(ASTContext& Context) {
    // std::cout << "default handleTU" << std::endl;
// }

//-----------------------------------------------------------------------------

// bool Action::BeginSourceFileAction(CompilerInstance& CI) {
//     std::cout << "Starting to process file"
//     << getCurrentFile().data() << std::endl;
//     return true;
// }
// std::unique_ptr<clang::ASTConsumer>
// Action::CreateASTConsumer(
//     clang::CompilerInstance &CI, llvm::StringRef InFile){
//         std::cout << "CreateASTConsumer() called" << std::endl;
//         return std::make_unique<Consumer>();
// }

//-----------------------------------------------------------------------------

// std::unique_ptr<FrontendAction>
// Factory::create() {
//     std::cout << "Factory created" << std::endl;
//     return std::make_unique<Action>();
// }
