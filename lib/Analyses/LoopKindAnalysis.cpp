#include <iostream>

#include "cxx-langstat/Analyses/LoopKindAnalysis.h"

using namespace clang::ast_matchers;

using ordered_json = nlohmann::ordered_json;


LoopKindAnalysis::LoopKindAnalysis(){
    std::cout << "LKA ctor\n";
}
LoopKindAnalysis::~LoopKindAnalysis(){
    std::cout << "LKA dtor\n";
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

    ordered_json loops;
    loops["for"] = ForMatches.size();
    loops["while"] = WhileMatches.size();
    loops["do-while"] = DoWhileMatches.size();
    loops["for-range"] = RangeBasedForMatches.size();
    Result = loops;
}

void LoopKindAnalysis::run(llvm::StringRef InFile, clang::ASTContext& Context){
    std::cout << "\033[32mCounting the different kinds of loops:\033[0m"
    << std::endl;
    extract(Context);
}
