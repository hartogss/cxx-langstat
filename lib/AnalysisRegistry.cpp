#include <iostream>

#include "cxx-langstat/AnalysisRegistry.h"
#include "cxx-langstat/AnalysisList.h"

#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"

class test {

};


AnalysisRegistry::AnalysisRegistry(clang::ASTContext& Context) : Context(Context), EnabledAnalyses() {
    Setup();
}
void AnalysisRegistry::Setup(){
    auto t = std::make_unique<test>();
    auto t1 =
        std::make_unique<TemplateInstantiationAnalysis>(llvm::StringRef("weirdfile.cpp"), Context);
    AnalysisMapping.emplace("tia", std::move(t1));
}

void AnalysisRegistry::setEnabledAnalyses(std::string S){
    AnalysisList A(S);
    std::cout << A.Items.size() << std::endl;
    EnabledAnalyses = A;

}

void AnalysisRegistry::runEnabledAnalyses(){
    for(auto an : EnabledAnalyses.Items){
        std::cout << "1" << std::endl;
        auto a = an.Name.str();
        AnalysisMapping[a]->run();
    }
}
