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
    Matches<clang::FunctionDecl> FunctionDeclsWithCopy;
    Matches<clang::FunctionDecl> FunctionDeclsWithlValueRef;
    Matches<clang::FunctionDecl> FunctionDeclsWithConstlValueRef;
    Matches<clang::FunctionDecl> FunctionDeclsWithrValueRef;
    Matches<clang::FunctionTemplateDecl> FunctionTemplatesDeclsWithCopy;
    Matches<clang::FunctionTemplateDecl> FunctionTemplatesDeclsWithlValueRef;
    Matches<clang::FunctionTemplateDecl> FunctionTemplatesDeclsWithConstlValueRef;
    Matches<clang::FunctionTemplateDecl> FunctionTemplatesDeclsWithrValueRef;
    Matches<clang::FunctionTemplateDecl> FunctionTemplatesDeclsWithUniversalRef;
    Matches<clang::ParmVarDecl> CopyParmDecls;
    Matches<clang::ParmVarDecl> lValueRefParmDecls;
    Matches<clang::ParmVarDecl> ConstlValueRefParmDecls;
    Matches<clang::ParmVarDecl> rValueRefParmDecls;
    Matches<clang::ParmVarDecl> UniversalRefParmDecls;
};

#endif // MOVESEMANTICSANALYSIS_H

//-----------------------------------------------------------------------------
