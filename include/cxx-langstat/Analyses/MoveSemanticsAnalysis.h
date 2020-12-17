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
    void gatherStatistics(std::string text, const Matches<T>& Matches);
    Matches<clang::FunctionDecl> FunctionDeclsWithCopy;
    Matches<clang::FunctionDecl> FunctionDeclsWithlValueRef;
    Matches<clang::FunctionDecl> FunctionDeclsWithConstlValueRef;
    Matches<clang::FunctionDecl> FunctionDeclsWithrValueRef;
    Matches<clang::ParmVarDecl> CopyParmDecls;
    Matches<clang::ParmVarDecl> lValueRefParmDecls;
    Matches<clang::ParmVarDecl> ConstlValueRefParmDecls;
    Matches<clang::ParmVarDecl> rValueRefParmDecls;
};

#endif // MOVESEMANTICSANALYSIS_H

//-----------------------------------------------------------------------------
