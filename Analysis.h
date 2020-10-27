#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

// should be abstract class
// need analysis object since we want other analysis to inherit interface
class Analysis {
public:
    Analysis(clang::tooling::ClangTool Tool);
    // step 1: extraction

    //step 2: compute stats
    //step 3: visualization (for later)

    // combine
    void run();

    // std::string name;
    clang::tooling::ClangTool _Tool;

};

#endif /* ANALYSIS_H */
