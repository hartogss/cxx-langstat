#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <nlohmann/json.hpp>

#include "BaseExtractor.h"

//-----------------------------------------------------------------------------
// Abstract Analysis class. Specific analyses should subclass this & implement its methods.

class Analysis {
public:
    Analysis(){
    }
    // should be made virtual in case concrete analyses need special destructors
    ~Analysis() = default;
    // Run analysis
    virtual void run(llvm::StringRef InFile, clang::ASTContext& Context)=0;
    // make private and use getters and setters
    llvm::StringRef InFile;
    BaseExtractor Extractor;
    clang::ASTContext* Context;
};

//-----------------------------------------------------------------------------

#endif // ANALYSIS_H
