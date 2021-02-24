#ifndef CONTAINERLIBANALYSIS_H
#define CONTAINERLIBANALYSIS_H

#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"

//-----------------------------------------------------------------------------

class ContainerLibAnalysis : public TemplateInstantiationAnalysis {
public:
    ContainerLibAnalysis();
    ~ContainerLibAnalysis(){
        std::cout << "CLA dtor\n";
    }
private:
    void processFeatures(nlohmann::ordered_json j) override;
    // JSON keys
    const std::string ContainerPrevalenceKey = "container type prevalence";
    const std::string ContainedTypesPrevalenceKey =
        "container instantiation type arguments";
};

//-----------------------------------------------------------------------------

#endif // CONTAINERLIBANALYSIS_H
