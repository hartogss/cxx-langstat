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
};

//-----------------------------------------------------------------------------

#endif // UTILITYLIBANALYSIS_H
