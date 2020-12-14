#ifndef LOOPDEPTHANALYSIS_H
#define LOOPDEPTHANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

// need analysis object since we want other analysis to inherit interface
class LoopDepthAnalysis : public Analysis {
public:
    LoopDepthAnalysis();
    ~LoopDepthAnalysis();
    void extract();
    void analyzeDepth(Matches<clang::Stmt> matches, std::vector<Matches<clang::Stmt>> Data);
    void analyzeLoopPrevalences(
        Matches<clang::Stmt> fs,
        Matches<clang::Stmt> ws,
        Matches<clang::Stmt> ds);
    // combine
    void run(llvm::StringRef InFile, clang::ASTContext& Context) override;
private:
    int MaxDepth;
};

//-----------------------------------------------------------------------------

#endif // LOOPDEPTHANALYSIS_H
