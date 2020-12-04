#ifndef STDLIBANALYSIS2_H
#define STDLIBANALYSIS2_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class StdlibAnalysis2 : public Analysis {
public:
    StdlibAnalysis2(clang::ASTContext& Context);
    void extract() override;
    void analyze() override;
    void run() override;
private:
    Matches<clang::Decl> VarDecls;
    Matches<clang::Decl> StdContainerVarDecls;
    Matches<clang::Decl> StdContainerFieldDecls;
    clang::ASTContext& Context;

};

//-----------------------------------------------------------------------------

#endif // STDLIBANALYSIS2_H
