#include <iostream>

#include "cxx-langstat/AnalysisRegistry.h"

//-----------------------------------------------------------------------------

AnalysisRegistry::AnalysisRegistry(CXXLangstatOptions Opts) : Options(Opts) {
    std::cout << "Registry ctor" << std::endl;
}
AnalysisRegistry::~AnalysisRegistry(){
    std::cout << "Registry dtor" << std::endl;
}
void AnalysisRegistry::createFreshAnalyses(){
    std::cout << "Creating analyses" << std::endl;
    // Example of where std::move could be used if std::make<"Analysis">
    // was defined explicitly as a variable
    // Create all analyses and give them to the registry

    // If some analyses are enabled
    if(!Options.EnabledAnalyses.Items.empty()){
        // std::cout << ConstexprAnalysis::b << std::endl;
        // AnalysisFactory::Create("cea");
        for(const auto& ab : Options.EnabledAnalyses.Items){
            auto analysis = AnalysisFactory::Create(ab.Name);
            if(analysis)
                Analyses.emplace_back(std::move(analysis));
            else
                std::cout << "Could not create analysis \"" << ab.Name << "\"\n";
        }
    } else { // otherwise run all analyses
        for(const auto& ab : AnalysisFactory::AnalyzerFactoryRegistry){
            auto analysis = ab.second();
            if(analysis)
                Analyses.emplace_back(std::move(analysis));
            else
                std::cout << "Could not create analysis \"" << ab.first << "\"\n";
        }
    }
}

//-----------------------------------------------------------------------------
