#ifndef TEMPLATEINSTANTIATIONANALYSIS_H
#define TEMPLATEINSTANTIATIONANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class TemplateInstantiationAnalysis : public Analysis {
public:
    TemplateInstantiationAnalysis(clang::ASTContext& Context);
    TemplateInstantiationAnalysis(clang::ASTContext& Context,
        clang::ast_matchers::internal::Matcher<clang::NamedDecl> Names);
    void extract() override;
    void analyze() override;
    void run() override;
private:
    clang::ast_matchers::DeclarationMatcher ClassInstMatcher;
    Matches<clang::ClassTemplateSpecializationDecl> ClassInsts;
    Matches<clang::FunctionDecl> FuncInsts;
    Matches<clang::VarTemplateSpecializationDecl> VarInsts;
    bool analyzeFuncInsts = true;
    bool analyzeVarInsts = true;
};

//-----------------------------------------------------------------------------

#endif // TEMPLATEINSTANTIATIONANALYSIS_H
