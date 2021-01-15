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
private:
    Matches<clang::Decl> ClassWithStaticMemberDecls;
    Matches<clang::Decl> ConstexprFunctionDecls;
    Matches<clang::Decl> VariableTemplateDecls;
    void extractFeatures();
    template<typename T>
    void gatherData(std::string VTKind, const Matches<T>& Matches);
    void analyzeFeatures() override;
    void processJSON() override;
};

//-----------------------------------------------------------------------------

#endif // VARIABLETEMPLATEANALYSIS_H
