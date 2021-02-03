#ifndef STDLIBANALYSIS2_H
#define STDLIBANALYSIS2_H

#include "cxx-langstat/Analysis.h"
#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"

//-----------------------------------------------------------------------------
// A standard library analysis is a template instantiation analysis.
class StdlibAnalysis2 : public TemplateInstantiationAnalysis {
public:
    StdlibAnalysis2();
    ~StdlibAnalysis2(){
        std::cout << "SLA2 dtor\n";
    }
private:
    void processFeatures(nlohmann::ordered_json j) override;
};

//-----------------------------------------------------------------------------

#endif // STDLIBANALYSIS2_H
