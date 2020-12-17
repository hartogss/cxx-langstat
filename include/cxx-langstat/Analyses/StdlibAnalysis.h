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
