#ifndef VARIABLETEMPLATEANALYSIS_H
#define VARIABLETEMPLATEANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class VariableTemplateAnalysis : public Analysis {
public:
    VariableTemplateAnalysis(llvm::StringRef InFile,
        clang::ASTContext& Context);
    void extract() override;
    void analyze() override;
    void run() override;
private:
    Matches<clang::Decl> ClassWithStaticMemberDecls;
    Matches<clang::Decl> ConstexprFunctionDecls;
    Matches<clang::Decl> VariableTemplateDecls;
};

//-----------------------------------------------------------------------------

#endif // VARIABLETEMPLATEANALYSIS_H
