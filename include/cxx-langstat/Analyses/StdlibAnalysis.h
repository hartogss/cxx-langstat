#ifndef STDLIBANALYSIS_H
#define STDLIBANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class StdlibAnalysis : public Analysis {
public:
    StdlibAnalysis()=default;
    void extract();
    void analyze();
    void run(llvm::StringRef InFile, clang::ASTContext& Context) override;
private:
    Matches<clang::Decl> VarDecls;
    Matches<clang::Decl> StdContainerVarDecls;
    Matches<clang::Decl> StdContainerFieldDecls;
};

//-----------------------------------------------------------------------------

#endif // STDLIBANALYSIS_H
