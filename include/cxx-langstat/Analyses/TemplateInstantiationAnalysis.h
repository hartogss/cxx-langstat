#ifndef TEMPLATEINSTANTIATIONANALYSIS_H
#define TEMPLATEINSTANTIATIONANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class TemplateInstantiationAnalysis final : public Analysis {
public:
    TemplateInstantiationAnalysis();
    TemplateInstantiationAnalysis(
        clang::ast_matchers::internal::Matcher<clang::NamedDecl> Names);
    void extract();
    void run(llvm::StringRef InFile, clang::ASTContext& Context) override;
    bool analyzeFuncInsts = true;
    bool analyzeVarInsts = true;
private:
    clang::ast_matchers::DeclarationMatcher ClassInstMatcher;
    Matches<clang::ClassTemplateSpecializationDecl> ClassImplicitInsts;
    Matches<clang::ClassTemplateSpecializationDecl> ClassExplicitInsts;
    Matches<clang::DeclaratorDecl> ImplicitInsts;
    Matches<clang::FunctionDecl> FuncInsts;
    Matches<clang::VarTemplateSpecializationDecl> VarInsts;
    template<typename T>
    std::string getInstantiationLocation(const Match<T>& Match, bool isImplicit);
    std::string getInstantiationLocation
        (const Match<clang::ClassTemplateSpecializationDecl>& Match,
            bool isImplicit);
    template<typename T>
    void gatherStats(Matches<T>& Insts, std::string InstKind, bool AreImplicit,
        std::ofstream&& file);
};

//-----------------------------------------------------------------------------

#endif // TEMPLATEINSTANTIATIONANALYSIS_H
