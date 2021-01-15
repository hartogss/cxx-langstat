#ifndef LOOPDEPTHANALYSIS_H
#define LOOPDEPTHANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class LoopDepthAnalysis : public Analysis {
public:
    LoopDepthAnalysis() : Analysis(){
        MaxDepth=4;
        std::cout<<"LDA ctor\n";
    }
    ~LoopDepthAnalysis(){
        std::cout << "LDA dtor\n";
    }
private:
    int MaxDepth;
    Matches<clang::Stmt> TopLevelLoops;
    std::vector<Matches<clang::Stmt>> LoopsOfDepth;
    void extractFeatures();
    void analyzeFeatures() override;
    void processJSON() override;
};

//-----------------------------------------------------------------------------

#endif // LOOPDEPTHANALYSIS_H
