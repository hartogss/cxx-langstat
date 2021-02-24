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
private:
    void processFeatures(nlohmann::ordered_json j) override;
    // JSON keys
    const std::string UtilityPrevalenceKey = "utility type prevalence";
    const std::string UtilitiedTypesPrevalenceKey =
        "utility instantiation type arguments";
};

//-----------------------------------------------------------------------------

#endif // UTILITYLIBANALYSIS_H
