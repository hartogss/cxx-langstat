#ifndef ALGORITHMLIBRARYANALYSIS_H
#define ALGORITHMLIBRARYANALYSIS_H

#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"

//-----------------------------------------------------------------------------
// A standard library analysis is a template instantiation analysis.
class AlgorithmLibraryAnalysis : public TemplateInstantiationAnalysis {
public:
    AlgorithmLibraryAnalysis();
    ~AlgorithmLibraryAnalysis(){
        std::cout << "ALA dtor\n";
    }
private:
    void processFeatures(nlohmann::ordered_json j) override;
};

//-----------------------------------------------------------------------------

#endif // ALGORITHMLIBRARYANALYSIS_H
