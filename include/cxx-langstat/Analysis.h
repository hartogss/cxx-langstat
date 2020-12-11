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

    //step 3: visualization (for later)
    // combine
    virtual void run(llvm::StringRef InFile, clang::ASTContext& Context)=0;
    // std::string name;
    virtual void print(){
        std::cout << "rpint" << std::endl;
    }

    llvm::StringRef InFile;
    BaseExtractor Extractor;
};

//-----------------------------------------------------------------------------

#endif // ANALYSIS_H
