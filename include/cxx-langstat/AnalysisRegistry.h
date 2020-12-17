#ifndef ANALYSISREGISTRY_H
#define ANALYSISREGISTRY_H

#include <memory>
#include "cxx-langstat/Analysis.h"

#include "cxx-langstat/AnalysisList.h"

class AnalysisRegistry {
public:
    AnalysisRegistry();
    ~AnalysisRegistry();
    // Don't know why I have to go the extra mile here and not just use std::map
    std::vector<std::unique_ptr<Analysis>> Analyses;
    void setEnabledAnalyses(std::string);
    AnalysisList EnabledAnalyses;
    std::vector<std::string> Abbrev;
private:
    void createAllAnalyses();
};

#endif // ANALYSISREGISTRY_H
