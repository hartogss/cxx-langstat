#include <iostream>

#include "cxx-langstat/LoopKindAnalysis.h"

using namespace clang::tooling;
using namespace clang::ast_matchers;

LoopKindAnalysis::LoopKindAnalysis(clang::ASTContext& Context) : Analysis(Context){

}
void LoopKindAnalysis::extract(){

    // Analysis of prevalence of different loop statement, i.e. comparing for, while etc.
    auto ForMatches = Extr.extract("fs1", forStmt().bind("fs1"));
    auto WhileMatches = Extr.extract("ws1", whileStmt().bind("ws1"));
    auto DoWhileMatches = Extr.extract("ds1", doStmt().bind("ds1"));
    auto RangeBasedForMatches = Extr.extract("forrange", cxxForRangeStmt().bind("forrange"));

    unsigned total = ForMatches.size() + WhileMatches.size() + DoWhileMatches.size() + RangeBasedForMatches.size();
    std::cout << ForMatches.size() << "/" << total << " are for loops" << std::endl;
    std::cout << WhileMatches.size() << "/" << total << " are while loops" << std::endl;
    std::cout << DoWhileMatches.size() << "/" << total << " are do-while loops" << std::endl;
    std::cout << RangeBasedForMatches.size() << "/" << total << " are range-based for loops" << std::endl;
}
void LoopKindAnalysis::analyze(){

}
void LoopKindAnalysis::run(){
    std::cout << "\033[32mCounting the different kinds of loops:\033[0m" << std::endl;
    extract();
}
