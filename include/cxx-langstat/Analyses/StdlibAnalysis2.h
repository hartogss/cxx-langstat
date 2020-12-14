#ifndef STDLIBANALYSIS2_H
#define STDLIBANALYSIS2_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class StdlibAnalysis2 : public Analysis {
public:
    StdlibAnalysis2()=default;
    void extract();
    void analyze();
    void run(llvm::StringRef InFile, clang::ASTContext& Context) override;
private:
    Matches<clang::Decl> VarDecls;
    Matches<clang::Decl> StdContainerVarDecls;
    Matches<clang::Decl> StdContainerFieldDecls;
};

//-----------------------------------------------------------------------------

#endif // STDLIBANALYSIS2_H
