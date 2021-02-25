#ifndef LOOPDEPTHANALYSIS_H
#define LOOPDEPTHANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class LoopDepthAnalysis : public Analysis {
public:
    LoopDepthAnalysis(unsigned MaxDepth = 5) : MaxDepth(MaxDepth){
        std::cout<<"LDA ctor\n";
    }
    ~LoopDepthAnalysis(){
        std::cout << "LDA dtor\n";
    }
    std::string getShorthand() override {
        return ShorthandName;
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
    static constexpr auto ShorthandName = "lda";
};

//-----------------------------------------------------------------------------

#endif // LOOPDEPTHANALYSIS_H
