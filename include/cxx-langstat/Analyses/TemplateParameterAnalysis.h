#ifndef TEMPLATEPARAMETERANALYSIS_H
#define TEMPLATEPARAMETERANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class TemplateParameterAnalysis : public Analysis {
public:
    TemplateParameterAnalysis(){
        std::cout << "TPA ctor\n";
    }
    ~TemplateParameterAnalysis(){
        std::cout << "TPA dtor\n";
    }
private:
    void extractFeatures();
    void gatherStatistics(const Matches<clang::Decl>& Matches,
        std::string TemplateKind);
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
    Matches<clang::Decl> ClassTemplates;
    Matches<clang::Decl> FunctionTemplates;
    Matches<clang::Decl> VariableTemplates;
    Matches<clang::Decl> AliasTemplates;
};

//-----------------------------------------------------------------------------

#endif // TEMPLATEPARAMETERANALYSIS_H
