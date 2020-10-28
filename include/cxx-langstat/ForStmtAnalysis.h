#ifndef FORSTMTANALYSIS_H
#define FORSTMTANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "Analysis.h"

// need analysis object since we want other analysis to inherit interface
class ForStmtAnalysis : Analysis {
public:
    ForStmtAnalysis(clang::tooling::ClangTool Tool, int MaxDepthOption);
    // step 0: createMatcher(s) ?
    // step 1: extraction
    void extract();
    //step 2: compute stats
    void analyze(std::vector<int> Data);
    //step 3: visualization (for later)
    // combine
    void run();

    // std::string name;
    int MaxDepth;
private:
    class Extractor Extr;
};

#endif /* FORSTMTANALYSIS_H */
