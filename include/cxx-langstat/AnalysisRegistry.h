#ifndef ANALYSISREGISTRY_H
#define ANALYSISREGISTRY_H

#include <map>
#include <memory>
#include "cxx-langstat/Analysis.h"

#include "cxx-langstat/AnalysisList.h"
#include "cxx-langstat/Analyses/LoopDepthAnalysis.h"


class AnalysisRegistry {
public:
    AnalysisRegistry();
    AnalysisRegistry(std::string EAS);
    // ~AnalysisRegistry();
    void runEnabledAnalyses(llvm::StringRef InFile, clang::ASTContext& Context);
    LoopDepthAnalysis LDA;
    std::vector<std::unique_ptr<Analysis>> Analyses;
    std::map<std::string, std::unique_ptr<Analysis>> AnalysisMapping;
    void setEnabledAnalyses(std::string);
    AnalysisList EnabledAnalyses;

    std::vector<std::string> Abbrev;


private:
    void createAllAnalyses();
    // clang::ASTContext& Context;
};

#endif // ANALYSISREGISTRY_H
