#ifndef STDLIBANALYSIS2_H
#define STDLIBANALYSIS2_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class StdlibAnalysis2 : public Analysis {
public:
    StdlibAnalysis2()=default;
    void run(llvm::StringRef InFile, clang::ASTContext& Context) override;
};

//-----------------------------------------------------------------------------

#endif // STDLIBANALYSIS2_H
