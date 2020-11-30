#include <iostream>

#include "cxx-langstat/Analyses/LoopKindAnalysis.h"

using namespace clang::ast_matchers;

LoopKindAnalysis::LoopKindAnalysis(clang::ASTContext& Context) : Analysis(Context){

}
void LoopKindAnalysis::extract(){

    // Analysis of prevalence of different loop statement, i.e. comparing for, while etc.
    auto ForMatches = Extr.extract("fs1", forStmt().bind("fs1"));
    auto WhileMatches = Extr.extract("ws1", whileStmt().bind("ws1"));
    auto DoWhileMatches = Extr.extract("ds1", doStmt().bind("ds1"));
    auto RangeBasedForMatches = Extr.extract("forrange", cxxForRangeStmt().bind("forrange"));

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
void LoopKindAnalysis::analyze(){

}
void LoopKindAnalysis::run(){
    std::cout << "\033[32mCounting the different kinds of loops:\033[0m"
    << std::endl;
    extract();
}
