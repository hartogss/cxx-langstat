#ifndef ANALYSISREGISTRY_H
#define ANALYSISREGISTRY_H

#include <map>
#include <memory>
#include "cxx-langstat/Analysis.h"

#include "cxx-langstat/AnalysisList.h"

class AnalysisRegistry {
public:
    AnalysisRegistry(clang::ASTContext& Context);
    void setEnabledAnalyses(std::string);
    void runEnabledAnalyses();
private:
    void Setup();
    AnalysisList EnabledAnalyses;
    std::map<std::string, std::unique_ptr<Analysis>> AnalysisMapping;
    clang::ASTContext& Context;
};

#endif // ANALYSISREGISTRY_H
