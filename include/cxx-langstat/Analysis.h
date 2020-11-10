#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"


#include "BaseExtractor.h"

//-----------------------------------------------------------------------------

// should be abstract class
// need analysis object since we want other analysis to inherit interface
class Analysis {
public:
    Analysis(clang::tooling::ClangTool Tool);
    // step 0: createMatcher(s) ?
    // step 1: extraction
    void extract();
    //step 2: compute stats
    void analyze();
    //step 3: visualization (for later)
    // combine
    void run();
    // std::string name;
protected:
    BaseExtractor Extr;
};

//-----------------------------------------------------------------------------

#endif /* ANALYSIS_H */
