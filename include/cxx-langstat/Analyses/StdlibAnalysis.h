#ifndef STDLIBANALYSIS_H
#define STDLIBANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class StdlibAnalysis : public Analysis {
public:
    StdlibAnalysis(){
        std::cout << "SLA ctor\n";
    }
    ~StdlibAnalysis(){
        std::cout << "SLA dtor\n";
    }
private:
    void extractFeatures();
    void analyzeFeatures() override;
    void processJSON() override;
    Matches<clang::Decl> VarDecls;
    Matches<clang::Decl> StdContainerVarDecls;
    Matches<clang::Decl> StdContainerFieldDecls;
};

//-----------------------------------------------------------------------------

#endif // STDLIBANALYSIS_H
