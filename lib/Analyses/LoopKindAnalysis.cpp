#include <iostream>

#include "cxx-langstat/Analyses/LoopKindAnalysis.h"

using namespace clang::ast_matchers;

LoopKindAnalysis::LoopKindAnalysis(){

}
void LoopKindAnalysis::extract(clang::ASTContext& Context){

    // Analysis of prevalence of different loop statement, i.e. comparing for, while etc.
    auto ForMatches = Extractor.extract(Context, "fs1", forStmt(isExpansionInMainFile())
    .bind("fs1"));
    auto WhileMatches = Extractor.extract(Context, "ws1", whileStmt(isExpansionInMainFile())
    .bind("ws1"));
    auto DoWhileMatches = Extractor.extract(Context, "ds1", doStmt(isExpansionInMainFile())
    .bind("ds1"));
    auto RangeBasedForMatches = Extractor.extract(Context, "forrange",
    cxxForRangeStmt(isExpansionInMainFile())
    .bind("forrange"));

    unsigned total = ForMatches.size() + WhileMatches.size()
        + DoWhileMatches.size() + RangeBasedForMatches.size();
    std::cout << ForMatches.size() << "/" << total
    << " are for loops\n"
    << WhileMatches.size() << "/" << total
    << " are while loops\n"
    << DoWhileMatches.size() << "/" << total
    << " are do-while loops\n"
    << RangeBasedForMatches.size() << "/" << total
    << " are range-based for loops\n";
}

void LoopKindAnalysis::run(llvm::StringRef InFile, clang::ASTContext& Context){
    std::cout << "\033[32mCounting the different kinds of loops:\033[0m"
    << std::endl;
    extract(Context);
}
