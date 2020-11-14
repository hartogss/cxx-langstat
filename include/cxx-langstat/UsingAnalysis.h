#ifndef USINGANALYSIS_H
#define USINGANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

// need analysis object since we want other analysis to inherit interface
class UsingAnalysis : public Analysis {
public:
    UsingAnalysis(clang::tooling::ClangTool Tool);
    void extract() override;
    void analyze() override;
    void run() override;
private:
    Matches<clang::Decl> TypeDefDecls;
    Matches<clang::Decl> TypeAliasDecls;
};

//-----------------------------------------------------------------------------

#endif /* USINGANALYSIS_H */
