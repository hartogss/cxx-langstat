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
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    void extractFeatures();
    void gatherData(const Matches<clang::Decl>& Matches,
        std::string TemplateKind);
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
    Matches<clang::Decl> ClassTemplates;
    Matches<clang::Decl> FunctionTemplates;
    Matches<clang::Decl> VariableTemplates;
    Matches<clang::Decl> AliasTemplates;
    //
    static constexpr auto ShorthandName = "tpa";
};

// Helper structs we create to later write to JSON files using helper functions.
// Advantage: can be more easily extended if we want to extract more rich features
// and/or compute statistics of.
struct TemplateParms {
    int Nontype = 0;
    int Type = 0;
    int Template = 0;
    int NumParms() {
        return Nontype+Type+Template;
    }
};
struct Template {
    int Location;
    bool UsesParamPack;
    TemplateParms Parms;
};

//-----------------------------------------------------------------------------

#endif // TEMPLATEPARAMETERANALYSIS_H
