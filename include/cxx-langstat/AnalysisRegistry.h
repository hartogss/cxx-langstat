#ifndef ANALYSISREGISTRY_H
#define ANALYSISREGISTRY_H

#include <map>
#include <memory>
#include "cxx-langstat/Analysis.h"

#include "cxx-langstat/AnalysisList.h"

class AnalysisRegistry {
public:
    AnalysisRegistry(std::string EAS);
    // ~AnalysisRegistry();
    void runEnabledAnalyses(llvm::StringRef InFile, clang::ASTContext& Context);
private:
    void createAllAnalyses();
    void setEnabledAnalyses(std::string);
    AnalysisList EnabledAnalyses;
    std::map<std::string, Analysis*> AnalysisMapping;
    // clang::ASTContext& Context;
};

#endif // ANALYSISREGISTRY_H
