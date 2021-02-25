#ifndef UTILITYLIBANALYSIS_H
#define UTILITYLIBANALYSIS_H

#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"

//-----------------------------------------------------------------------------

class UtilityLibAnalysis : public TemplateInstantiationAnalysis {
public:
    UtilityLibAnalysis();
    ~UtilityLibAnalysis(){
        std::cout << "ULA dtor\n";
    }
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    void processFeatures(nlohmann::ordered_json j) override;
    // JSON keys
    const std::string UtilityPrevalenceKey = "utility type prevalence";
    const std::string UtilitiedTypesPrevalenceKey =
        "utility instantiation type arguments";
        //
    static constexpr auto ShorthandName = "ula";
};

//-----------------------------------------------------------------------------

#endif // UTILITYLIBANALYSIS_H
