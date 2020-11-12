#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"

#include "BaseExtractor.h"

//-----------------------------------------------------------------------------

// Abstract Analysis class. Specific analyses should subclass this & implement its methods.

class Analysis {
public:
    Analysis(clang::tooling::ClangTool Tool) : Extr(BaseExtractor(Tool)){

    }
    ~Analysis() = default; // should be made virtual in case concrete analyses need special destructors
    // step 0: createMatcher(s) ?
    // step 1: extraction
    virtual void extract()=0;
    //step 2: compute stats
    virtual void analyze()=0;
    //step 3: visualization (for later)
    // combine
    virtual void run()=0;
    // std::string name;
protected:
    BaseExtractor Extr;
};

//-----------------------------------------------------------------------------

#endif /* ANALYSIS_H */
