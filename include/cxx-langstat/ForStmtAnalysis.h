#ifndef FORSTMTANALYSIS_H
#define FORSTMTANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "Analysis.h"

// need analysis object since we want other analysis to inherit interface
class ForStmtAnalysis : public Analysis {
public:
    ForStmtAnalysis(clang::tooling::ClangTool Tool, int MaxDepthOption);
    ~ForStmtAnalysis();
    // step 0: createMatcher(s)/getMatchers? idea is to have library of often-used
    // matchers or maybe even some datastructure of important matches itself

    // step 1: extraction
    void extract();
    //step 2: compute stats
    void analyze(std::vector<int> Data);
    //step 3: visualization (for later)

    // combine
    void run();

    // std::string name;
private:
    int MaxDepth;
};

#endif /* FORSTMTANALYSIS_H */