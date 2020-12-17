#include <iostream>

#include "cxx-langstat/AnalysisRegistry.h"
#include "cxx-langstat/AnalysisList.h"

#include "cxx-langstat/Analyses/CyclomaticComplexityAnalysis.h"
#include "cxx-langstat/Analyses/LoopDepthAnalysis.h"
#include "cxx-langstat/Analyses/LoopKindAnalysis.h"
#include "cxx-langstat/Analyses/MoveSemanticsAnalysis.h"
#include "cxx-langstat/Analyses/StdlibAnalysis.h"
#include "cxx-langstat/Analyses/StdlibAnalysis2.h"
#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"
#include "cxx-langstat/Analyses/TemplateParameterAnalysis.h"
#include "cxx-langstat/Analyses/UsingAnalysis.h"
#include "cxx-langstat/Analyses/VariableTemplateAnalysis.h"


AnalysisRegistry::AnalysisRegistry() {
    std::cout << "Registry ctor" << std::endl;
    createAllAnalyses();
}
AnalysisRegistry::~AnalysisRegistry(){
    std::cout << "Registry dtor" << std::endl;
}
void AnalysisRegistry::createAllAnalyses(){
    std::cout << "Creating analyses" << std::endl;
    // Example of where std::move could be used if std::make<"Analysis">
    // was defined explicitly as a variable
    // Create all analyses and give them to the registry
    Analyses.emplace_back(std::make_unique<CyclomaticComplexityAnalysis>());
    Analyses.emplace_back(std::make_unique<LoopDepthAnalysis>());
    Analyses.emplace_back(std::make_unique<LoopKindAnalysis>());
    Analyses.emplace_back(std::make_unique<MoveSemanticsAnalysis>());
    Analyses.emplace_back(std::make_unique<StdlibAnalysis>());
    Analyses.emplace_back(std::make_unique<StdlibAnalysis2>());
    Analyses.emplace_back(std::make_unique<TemplateInstantiationAnalysis>());
    Analyses.emplace_back(std::make_unique<TemplateParameterAnalysis>());
    Analyses.emplace_back(std::make_unique<UsingAnalysis>());
    Analyses.emplace_back(std::make_unique<VariableTemplateAnalysis>());

    Abbrev.emplace_back("cca");
    Abbrev.emplace_back("lda");
    Abbrev.emplace_back("lka");
    Abbrev.emplace_back("msa");
    Abbrev.emplace_back("sla");
    Abbrev.emplace_back("sla2");
    Abbrev.emplace_back("tia");
    Abbrev.emplace_back("tpa");
    Abbrev.emplace_back("ua");
    Abbrev.emplace_back("vta");

}

void AnalysisRegistry::setEnabledAnalyses(std::string EAS){
    std::cout << "Enabling analyses: ";
    AnalysisList List(EAS);
    std::cout << "Number of analyses: " << List.Items.size() << std::endl;
    EnabledAnalyses = List; //why not use copy constructor,move ctor
}
