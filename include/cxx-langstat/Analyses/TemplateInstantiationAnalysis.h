#ifndef TEMPLATEINSTANTIATIONANALYSIS_H
#define TEMPLATEINSTANTIATIONANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class TemplateInstantiationAnalysis : public Analysis {
public:
    TemplateInstantiationAnalysis(llvm::StringRef InFile,
        clang::ASTContext& Context);
    TemplateInstantiationAnalysis(llvm::StringRef InFile,
        clang::ASTContext& Context,
        clang::ast_matchers::internal::Matcher<clang::NamedDecl> Names);
    void extract() override;
    void analyze() override;
    void run() override;
    bool analyzeFuncInsts = true;
    bool analyzeVarInsts = true;
private:
    clang::ast_matchers::DeclarationMatcher ClassInstMatcher;
    Matches<clang::ClassTemplateSpecializationDecl> ClassInsts;
    Matches<clang::DeclaratorDecl> ImplicitInsts;
    Matches<clang::FunctionDecl> FuncInsts;
    Matches<clang::VarTemplateSpecializationDecl> VarInsts;
    template<typename T>
    std::string getInstantiationLocation(const Match<T>& Match);
    std::string getInstantiationLocation(const Match<clang::FunctionDecl>& Match);
    std::string getInstantiationLocation(const Match<clang::VarTemplateSpecializationDecl>& Match);
    template<typename T>
    void gatherStats(Matches<T>& Insts, std::ofstream&& file);
};

//-----------------------------------------------------------------------------

#endif // TEMPLATEINSTANTIATIONANALYSIS_H
