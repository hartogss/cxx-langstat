#ifndef TESTANALYSIS_H
#define TESTANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

// need analysis object since we want other analysis to inherit interface
class TestAnalysis : public Analysis {
public:
    TestAnalysis(clang::tooling::ClangTool Tool);
    ~TestAnalysis();
    // step 0: createMatcher(s)/getMatchers? idea is to have library of often-used
    // matchers or maybe even some datastructure of important matches itself
    // step 1: extraction
    void extract();
    //step 2: compute stats
    //step 3: visualization (for later)
    // combine
    void run();
    // std::string name;
};

void runclangtidy();

//-----------------------------------------------------------------------------

#endif /* TESTANALYSIS_H */
