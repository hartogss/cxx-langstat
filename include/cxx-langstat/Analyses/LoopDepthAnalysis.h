#ifndef LOOPDEPTHANALYSIS_H
#define LOOPDEPTHANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class LoopDepthAnalysis : public Analysis {
public:
    LoopDepthAnalysis() : Analysis(){
                std::cout<<"LDA ctor\n";
    }
    ~LoopDepthAnalysis(){
        std::cout << "LDA dtor\n";
    }
    void extract();
    void analyzeDepth(Matches<clang::Stmt> matches, std::vector<Matches<clang::Stmt>> Data);
    // combine
    void run(llvm::StringRef InFile, clang::ASTContext& Context) override;
private:
    int MaxDepth;

};

//-----------------------------------------------------------------------------

#endif // LOOPDEPTHANALYSIS_H
