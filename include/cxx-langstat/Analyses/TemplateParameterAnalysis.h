#ifndef TEMPLATEPARAMETERANALYSIS_H
#define TEMPLATEPARAMETERANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class TemplateParameterAnalysis : public Analysis {
public:
    TemplateParameterAnalysis()=default;
    void extract();
    void analyze();
    void run(llvm::StringRef InFile, clang::ASTContext& Context) override;
private:
    Matches<clang::Decl> ClassTemplates;
    Matches<clang::Decl> ClassTemplateNonTypeParameters;
    Matches<clang::Decl> ClassTemplateTypeParameters;
    Matches<clang::Decl> ClassTemplateTemplateParameters;
    Matches<clang::Decl> FunctionTemplates;
    Matches<clang::Decl> FunctionTemplateNonTypeParameters;
    Matches<clang::Decl> FunctionTemplateTypeParameters;
    Matches<clang::Decl> FunctionTemplateTemplateParameters;
    Matches<clang::Decl> VariableTemplates;
    Matches<clang::Decl> VariableTemplateNonTypeParameters;
    Matches<clang::Decl> VariableTemplateTypeParameters;
    Matches<clang::Decl> VariableTemplateTemplateParameters;
    Matches<clang::Decl> AliasTemplates;
    Matches<clang::Decl> AliasTemplateNonTypeParameters;
    Matches<clang::Decl> AliasTemplateTypeParameters;
    Matches<clang::Decl> AliasTemplateTemplateParameters;
};

//-----------------------------------------------------------------------------

#endif // TEMPLATEPARAMETERANALYSIS_H
