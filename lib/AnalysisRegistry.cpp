#include <iostream>

#include "cxx-langstat/AnalysisRegistry.h"
#include "cxx-langstat/AnalysisList.h"

#include "cxx-langstat/Analyses/LoopDepthAnalysis.h"
#include "cxx-langstat/Analyses/LoopKindAnalysis.h"
#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"


AnalysisRegistry::AnalysisRegistry() {
    std::cout << "Registry ctor" << std::endl;
    createAllAnalyses();
}
AnalysisRegistry::~AnalysisRegistry(){
    std::cout << "Registry dtor" << std::endl;
}
void AnalysisRegistry::createAllAnalyses(){
    std::cout << "Creating analyses" << std::endl;
    Analyses.emplace_back(std::make_unique<LoopDepthAnalysis>()); // std::move
    Analyses.emplace_back(std::make_unique<LoopKindAnalysis>());
    Analyses.emplace_back(std::make_unique<TemplateInstantiationAnalysis>());
    Abbrev.emplace_back("lda");
    Abbrev.emplace_back("lka");
    Abbrev.emplace_back("tia");
}

void AnalysisRegistry::setEnabledAnalyses(std::string EAS){
    std::cout << "Enabling analyses: ";
    AnalysisList List(EAS);
    std::cout << "Number of analyses: " << List.Items.size() << std::endl;
    EnabledAnalyses = List; //why not use copy constructor,move ctor
}
