#ifndef ANALYSISREGISTRY_H
#define ANALYSISREGISTRY_H

#include <memory>
#include "cxx-langstat/Analysis.h"
#include "cxx-langstat/AnalysisList.h"

struct CXXLangstatOptions {
    CXXLangstatOptions(std::string AnalysesString,
        std::string OutDirectory) :
        OutDirectory(OutDirectory),
        EnabledAnalyses(AnalysisList(AnalysesString)){
            auto& Items = EnabledAnalyses.Items;
            std::sort(Items.begin(), Items.end());
    }
    std::string OutDirectory;
    AnalysisList EnabledAnalyses;
};

class AnalysisRegistry {
public:
    AnalysisRegistry(CXXLangstatOptions Opts);
    ~AnalysisRegistry();
    // Don't know why I have to go the extra mile here and not just use std::map
    std::vector<std::unique_ptr<Analysis>> Analyses;
    CXXLangstatOptions Options;
private:
    void createAllAnalyses();
};


#endif // ANALYSISREGISTRY_H
