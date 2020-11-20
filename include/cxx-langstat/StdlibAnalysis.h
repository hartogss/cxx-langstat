#ifndef STDLIBANALYSIS_H
#define STDLIBANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class StdlibAnalysis : public Analysis {
public:
    StdlibAnalysis(clang::tooling::ClangTool Tool);
    void extract() override;
    void analyze() override;
    void run() override;
};

//-----------------------------------------------------------------------------

#endif // STDLIBANALYSIS_H
