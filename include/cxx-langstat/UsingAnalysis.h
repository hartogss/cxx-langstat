#ifndef USINGANALYSIS_H
#define USINGANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

// need analysis object since we want other analysis to inherit interface
class UsingAnalysis : public Analysis {
public:
    UsingAnalysis(clang::ASTContext& Context);
    void extract() override;
    void analyze() override;
    void run() override;
private:
    Matches<clang::Decl> TypedefDecls;
    Matches<clang::Decl> TypeAliasDecls;
    Matches<clang::Decl> TypeAliasTemplateDecls;
    Matches<clang::Decl> TypedefTemplateDecls;
    Matches<clang::Decl> td;
};

//-----------------------------------------------------------------------------

#endif // USINGANALYSIS_H
