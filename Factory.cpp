#include "cxx-langstat/Factory.h"
#include "cxx-langstat/Analysis.h"

// Zero-initialize static list, i.e., it is empty
std::map<std::string, AnalysisFactory::TCreateMethod>
    AnalysisFactory::AnalyzerFactoryRegistry;

//-----------------------------------------------------------------------------

bool AnalysisFactory::RegisterAnalysis(std::string name,
        TCreateMethod classFactoryFunction){
            AnalyzerFactoryRegistry[name] = classFactoryFunction;
            return true;
}
std::unique_ptr<Analysis> AnalysisFactory::Create(std::string name){
    if (auto it = AnalyzerFactoryRegistry.find(name); it != AnalyzerFactoryRegistry.end())
        return it->second(); // call the createFunc
    return nullptr;
}

//-----------------------------------------------------------------------------
