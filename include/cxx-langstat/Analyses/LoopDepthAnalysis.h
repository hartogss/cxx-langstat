#ifndef LOOPDEPTHANALYSIS_H
#define LOOPDEPTHANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class LoopDepthAnalysis : public Analysis {
public:
    LoopDepthAnalysis(){
        MaxDepth=4;
        std::cout<<"LDA ctor\n";
    }
    ~LoopDepthAnalysis(){
        std::cout << "LDA dtor\n";
    }
private:
    int MaxDepth;
    // All loop statements that are not contained in any other loops
    Matches<clang::Stmt> TopLevelLoops;
    // For each depth from 1 to MaxDepth, contains all loops with that depth
    std::vector<Matches<clang::Stmt>> LoopsOfDepth;
    void extractFeatures();
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
    void ResetAnalysis() override;
};

//-----------------------------------------------------------------------------

#endif // LOOPDEPTHANALYSIS_H
