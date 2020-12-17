#ifndef VARIABLETEMPLATEANALYSIS_H
#define VARIABLETEMPLATEANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class VariableTemplateAnalysis : public Analysis {
public:
    VariableTemplateAnalysis(){
        std::cout << "VTA ctor\n";
    }
    ~VariableTemplateAnalysis(){
        std::cout << "VTA dtor\n";
    }
    void extract();
    void gatherStatistics();
    void run(llvm::StringRef InFile, clang::ASTContext& Context) override;
private:
    Matches<clang::Decl> ClassWithStaticMemberDecls;
    Matches<clang::Decl> ConstexprFunctionDecls;
    Matches<clang::Decl> VariableTemplateDecls;
};

//-----------------------------------------------------------------------------

#endif // VARIABLETEMPLATEANALYSIS_H
