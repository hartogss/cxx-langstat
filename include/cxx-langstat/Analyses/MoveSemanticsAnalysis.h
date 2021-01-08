#ifndef MOVESEMANTICSANALYSIS_H
#define MOVESEMANTICSANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class MoveSemanticsAnalysis : public Analysis {
public:
    MoveSemanticsAnalysis(){
        std::cout << "MSA ctor\n";
    }
    ~MoveSemanticsAnalysis(){
        std::cout << "MSA dtor\n";
    }
    void run(llvm::StringRef InFile, clang::ASTContext& Context) override;
private:
    void extract();
    template<typename T>
    void gatherData(std::string DeclKind, std::string PassKind,
        const Matches<T>& Matches);
    Matches<clang::FunctionDecl> FuncsWithValueParm;
    Matches<clang::FunctionDecl> FuncsWithNonConstLValueRefParm;
    Matches<clang::FunctionDecl> FuncsWithConstLValueRefParm;
    Matches<clang::FunctionDecl> FuncsWithRValueRefParm;
    Matches<clang::FunctionDecl> FuncsWithUniversalRefParm;
    Matches<clang::FunctionTemplateDecl> FuncTemplatesWithValueParm;
    Matches<clang::FunctionTemplateDecl> FuncTemplatesWithNonConstLValueRefParm;
    Matches<clang::FunctionTemplateDecl> FuncTemplatesWithConstLValueRefParm;
    Matches<clang::FunctionTemplateDecl> FuncTemplatesWithRValueRefParm;
    Matches<clang::FunctionTemplateDecl> FuncTemplatesWithUniversalRefParm;
    Matches<clang::ParmVarDecl> ValueParms;
    Matches<clang::ParmVarDecl> NonConstLValueRefParms;
    Matches<clang::ParmVarDecl> ConstLValueRefParms;
    Matches<clang::ParmVarDecl> RValueRefParms;
    Matches<clang::ParmVarDecl> UniversalRefParms;
};

#endif // MOVESEMANTICSANALYSIS_H

//-----------------------------------------------------------------------------
