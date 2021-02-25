#ifndef UTILITYLIBANALYSIS_H
#define UTILITYLIBANALYSIS_H

#include "cxx-langstat/Analysis.h"
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
    static constexpr auto ShorthandName = "ula";
};

//-----------------------------------------------------------------------------

#endif // UTILITYLIBANALYSIS_H
