#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "BaseExtractor.h"

//-----------------------------------------------------------------------------
// Abstract Analysis class. Specific analyses should subclass this & implement its methods.

class Analysis {
public:
    Analysis(){
    }
    // should be made virtual in case concrete analyses need special destructors
    ~Analysis() = default;
    // step 0: createMatcher(s) ?
    // step 1: extraction

    // Run analysis
    virtual void run(llvm::StringRef InFile, clang::ASTContext& Context)=0;

    llvm::StringRef InFile;
    BaseExtractor Extractor;
    clang::ASTContext* Context;
};

//-----------------------------------------------------------------------------

#endif // ANALYSIS_H
