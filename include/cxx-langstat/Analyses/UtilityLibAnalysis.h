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
private:
    void processFeatures(nlohmann::ordered_json j) override;
    static std::unique_ptr<Analysis> Create(){
        return std::make_unique<UtilityLibAnalysis>();
    }
    static bool s_registered;
};

//-----------------------------------------------------------------------------

#endif // UTILITYLIBANALYSIS_H
